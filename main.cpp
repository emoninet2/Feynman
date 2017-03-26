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


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <pthread.h>
#include <mqueue.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <time.h>
#include <limits.h>
#include <fcntl.h>
#include <errno.h>

#include "MQTTClient.h"
#include "MQTTClientPersistence.h"


#define ADDRESS     "tcp://localhost:1883"
#define CLIENTID    "Feynman"
#define TOPIC       "Feynman"
#define PAYLOAD     "Hello World!"
#define QOS         1
#define TIMEOUT     10000L

pthread_t thread1;



volatile MQTTClient_deliveryToken deliveredtoken;

typedef struct{
    MQTTClient client;
    void (*cl)(void *context, char *cause);
    int (*ma)(void *context, char *topicName, int topicLen, MQTTClient_message *message);
    void (*dc)(void *context, MQTTClient_deliveryToken dt);
}MQTTClientData_t;

MQTTClientData_t myClient;

void process_mqtt_message(char *topicName, int topicLen, MQTTClient_message *message){
    
    char buffer[message->payloadlen];
    memcpy(buffer,message->payload, message->payloadlen);
    buffer[message->payloadlen] = '\0';

    
    if(!strcmp(topicName,"emon")){
        printf("the topic name is emon\r\n");
    }
    else if(!strcmp(topicName,"subscribe")){
        printf("the topic name is subscribe\r\n");
    }
    else if(!strcmp(topicName,"unsubscribe")){
        printf("the topic name is unsubscribe\r\n");
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
    
    
    printf("do do \r\n");
    if ((rc = MQTTClient_connect(myClient.client, &conn_opts)) != MQTTCLIENT_SUCCESS)
    {
        printf("Failed to connect, return code %d\n", rc);
        exit(-1);
    }
    
    pubmsg.payload = (void* )PAYLOAD;
    pubmsg.payloadlen = strlen(PAYLOAD);
    pubmsg.qos = QOS;
    pubmsg.retained = 0;
    MQTTClient_publishMessage(myClient.client, TOPIC, &pubmsg, &token);
    printf("Waiting for up to %d seconds for publication of %s\n"
            "on topic %s for client with ClientID: %s\n",
            (int)(TIMEOUT/1000), PAYLOAD, TOPIC, CLIENTID);
    rc = MQTTClient_waitForCompletion(myClient.client, token, TIMEOUT);
    printf("Message with delivery token %d delivered\n", token);
    
     // MQTTClient_subscribe(client, TOPIC, QOS);
    MQTTClient_subscribe(myClient.client, "emon", QOS);
    MQTTClient_subscribe(myClient.client, "feynman/radio/txAddr", QOS);
    MQTTClient_subscribe(myClient.client, "feynman/radio/send/#", QOS);
    
    while(1){

    }

    MQTTClient_disconnect(myClient.client, 10000);
    MQTTClient_destroy(&myClient.client);
}

/*
 * 
 */
int main(int argc, char** argv) {
    
    pthread_create(&thread1,NULL, mqtt_thread, (void*) NULL);

    while(1){

    }

    return (EXIT_SUCCESS);
}