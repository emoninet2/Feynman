/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   main.cpp
 * Author: emon1
 *
 * Created on March 27, 2017, 12:10 AM
 * Include Linker -lwiringPi -lpthread -lrt -lpaho-mqtt3c to build
 */


#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <inttypes.h>
#include <pthread.h>
#include <semaphore.h>
#include <mqueue.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <time.h>
#include <limits.h>
#include <fcntl.h>
#include <errno.h>

#include "MQTTClient.h"
#include "MQTTClientPersistence.h"

#include "NRF24L01p/NRF24L01p.h"



#define ADDRESS     "tcp://localhost:1883"
#define CLIENTID    "Feynman"
#define TOPIC       "Feynman"
#define PAYLOAD     "Hello World!"
#define QOS         1
#define TIMEOUT     10000L

pthread_t thread1;
pthread_t thread2;
pthread_t thread3;

volatile MQTTClient_deliveryToken deliveredtoken;



typedef struct{
    MQTTClient client;
    void (*cl)(void *context, char *cause);
    int (*ma)(void *context, char *topicName, int topicLen, MQTTClient_message *message);
    void (*dc)(void *context, MQTTClient_deliveryToken dt);
}MQTTClientData_t;

MQTTClientData_t myClient;

NRF24L01p::Payload_t TxPayload;
sem_t nrf24l01pTxMutex;
uint8_t nrf24l01pTxBuffer[32];

NRF24L01p Radio;
NRF24L01p::RadioConfig_t RadioConfig;
NRF24L01p::RxPipeConfig_t RxPipeConfig[6];

void NRF24L01p_RadioReset(){
    RadioConfig.DataReadyInterruptEnabled = 0;
    RadioConfig.DataSentInterruptFlagEnabled = 0;
    RadioConfig.MaxRetryInterruptFlagEnabled = 0;
    RadioConfig.Crc = NRF24L01p::CONFIG_CRC_16BIT;
    RadioConfig.AutoReTransmissionCount = 15;
    RadioConfig.AutoReTransmitDelayX250us = 15;
    RadioConfig.frequencyOffset = 2;
    RadioConfig.datarate = NRF24L01p::RF_SETUP_RF_DR_2MBPS;
    RadioConfig.RfPower = NRF24L01p::RF_SETUP_RF_PWR_0DBM;
    RadioConfig.PllLock = 0;
    RadioConfig.ContWaveEnabled = 0;
    RadioConfig.FeatureDynamicPayloadEnabled = 1;
    RadioConfig.FeaturePayloadWithAckEnabled = 1;
    RadioConfig.FeatureDynamicPayloadWithNoAckEnabled = 1;

    RxPipeConfig[0].address = 0xAABBCCDDEE;
    RxPipeConfig[1].address = 0x6565656501;
    RxPipeConfig[2].address = 0x6565656502;
    RxPipeConfig[3].address = 0x6565656503;
    RxPipeConfig[4].address = 0x6565656509;
    RxPipeConfig[5].address = 0x6565656505;
 
    int i;
    for(i=0;i<6;i++){
        RxPipeConfig[i].PipeEnabled = 1;
        RxPipeConfig[i].autoAckEnabled = 1;
        RxPipeConfig[i].dynamicPayloadEnabled = 1;
    }
        
    Radio.ResetConfigValues(&RadioConfig, RxPipeConfig);
}


void process_mqtt_message(char *topicName, int topicLen, MQTTClient_message *message){

    if(!strcmp(topicName,"emon")){
        printf("the topic name is emon\r\n");
    }
    else if(!strcmp(topicName,"feynman/radio/nrf24/send")){
        while(sem_wait(&nrf24l01pTxMutex));
        if(message->payloadlen < 32){
            TxPayload.length = message->payloadlen;
            memcpy(TxPayload.data,message->payload, message->payloadlen);
        }
        sem_post(&nrf24l01pTxMutex);
    }
    else if(!strcmp(topicName,"feynman/radio/nrf24/txAddr")){
        while(sem_wait(&nrf24l01pTxMutex));
        uint64_t addr;
        addr = strtoull((char*)message->payload , NULL, 16);
        TxPayload.address = addr;
        sem_post(&nrf24l01pTxMutex);
    }
}

void connlost(void *context, char *cause)
{
    printf("\nConnection lost\n");
    printf("     cause: %s\n", cause);
}

int msgarrvd(void *context, char *topicName, int topicLen, MQTTClient_message *message)
{
    char buffer[message->payloadlen];
    memcpy(buffer,message->payload, message->payloadlen);
    buffer[message->payloadlen] = '\0';
    printf("%s/%s\r\n", topicName, buffer);
    
    process_mqtt_message(topicName, topicLen, message);
    
    MQTTClient_freeMessage(&message);
    MQTTClient_free(topicName);
    return 1;
}

void delivered(void *context, MQTTClient_deliveryToken dt)
{
    //printf("Message with token value %d delivery confirmed\n", dt);
    deliveredtoken = dt;
}

void *mqtt_thread(void *ptr){

    myClient.cl = &connlost;
    myClient.ma = &msgarrvd;
    myClient.dc = &delivered;
    
    //MQTTClient client;
    MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
    MQTTClient_message pubmsg = MQTTClient_message_initializer;
    MQTTClient_deliveryToken token;
    int rc;

    MQTTClient_create(&myClient.client, ADDRESS, CLIENTID,
        MQTTCLIENT_PERSISTENCE_NONE, NULL);
    conn_opts.keepAliveInterval = 20;
    conn_opts.cleansession = 1;
    
    //MQTT setting callback handers
    MQTTClient_setCallbacks(myClient.client, NULL, myClient.cl, myClient.ma, myClient.dc);
    
    
    if ((rc = MQTTClient_connect(myClient.client, &conn_opts)) != MQTTCLIENT_SUCCESS)
    {
        printf("Failed to connect, return code %d\n", rc);
        exit(-1);
    }
    
    //pubmsg.payload = (void* )PAYLOAD;
    //pubmsg.payloadlen = strlen(PAYLOAD);
    //pubmsg.qos = QOS;
    //pubmsg.retained = 0;
    //MQTTClient_publishMessage(myClient.client, TOPIC, &pubmsg, &token);
    //printf("Waiting for up to %d seconds for publication of %s\n"
    //        "on topic %s for client with ClientID: %s\n",
    //        (int)(TIMEOUT/1000), PAYLOAD, TOPIC, CLIENTID);
    //rc = MQTTClient_waitForCompletion(myClient.client, token, TIMEOUT);
    //printf("Message with delivery token %d delivered\n", token);
    
     // MQTTClient_subscribe(client, TOPIC, QOS);
    MQTTClient_subscribe(myClient.client, "emon", QOS);
    MQTTClient_subscribe(myClient.client, "feynman/#", QOS);
    //MQTTClient_subscribe(myClient.client, "feynman/radio/send/#", QOS);
    
    while(1){

    }

    MQTTClient_disconnect(myClient.client, 10000);
    MQTTClient_destroy(&myClient.client);
}



void *nrf24l01p_thread(void *ptr){
    printf("RX NODE\r\n");
    NRF24L01p_RadioReset();

    
    while(1){
        while(sem_wait(&nrf24l01pTxMutex));
        if(*TxPayload.data != NULL){
            printf("gonna send [%d] %s\r\n", TxPayload.length, TxPayload.data);
            Radio.TransmitPayload(&TxPayload);
            *TxPayload.data = NULL;
        }
        sem_post(&nrf24l01pTxMutex);
    }
}


/*
 * 
 */
int main(int argc, char** argv) {
    
    TxPayload.UseAck = 1;
    TxPayload.address = 0x11223344EE;
    TxPayload.data = nrf24l01pTxBuffer; 
    *TxPayload.data = NULL;
    
    sem_init(&nrf24l01pTxMutex, 0, 1); // initialize mutex
    pthread_create(&thread1,NULL, mqtt_thread, (void*) NULL);
    pthread_create(&thread2,NULL, nrf24l01p_thread, (void*) NULL);
    
    while(1){

    }

    return (EXIT_SUCCESS);
}