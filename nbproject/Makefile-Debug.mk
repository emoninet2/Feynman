#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Environment
MKDIR=mkdir
CP=cp
GREP=grep
NM=nm
CCADMIN=CCadmin
RANLIB=ranlib
CC=gcc
CCC=g++
CXX=g++
FC=gfortran
AS=as

# Macros
CND_PLATFORM=GNU-Linux
CND_DLIB_EXT=so
CND_CONF=Debug
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/NRF24L01p/NRF24L01p.o \
	${OBJECTDIR}/NRF24L01p/NRF24L01pDriver.o \
	${OBJECTDIR}/NRF24L01p/port/RPI_WiringPi/NRF24L01pPort.o \
	${OBJECTDIR}/NRF24L01p/port/mbed/NRF24L01pPort.o \
	${OBJECTDIR}/NRF24L01p/port/stm32/NRF24L01pPort.o \
	${OBJECTDIR}/main.o


# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=
CXXFLAGS=

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/feynman

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/feynman: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${LINK.cc} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/feynman ${OBJECTFILES} ${LDLIBSOPTIONS} -lwiringPi -lpthread -lrt -lpaho-mqtt3c

${OBJECTDIR}/NRF24L01p/NRF24L01p.o: NRF24L01p/NRF24L01p.cpp
	${MKDIR} -p ${OBJECTDIR}/NRF24L01p
	${RM} "$@.d"
	$(COMPILE.cc) -g -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/NRF24L01p/NRF24L01p.o NRF24L01p/NRF24L01p.cpp

${OBJECTDIR}/NRF24L01p/NRF24L01pDriver.o: NRF24L01p/NRF24L01pDriver.cpp
	${MKDIR} -p ${OBJECTDIR}/NRF24L01p
	${RM} "$@.d"
	$(COMPILE.cc) -g -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/NRF24L01p/NRF24L01pDriver.o NRF24L01p/NRF24L01pDriver.cpp

${OBJECTDIR}/NRF24L01p/port/RPI_WiringPi/NRF24L01pPort.o: NRF24L01p/port/RPI_WiringPi/NRF24L01pPort.cpp
	${MKDIR} -p ${OBJECTDIR}/NRF24L01p/port/RPI_WiringPi
	${RM} "$@.d"
	$(COMPILE.cc) -g -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/NRF24L01p/port/RPI_WiringPi/NRF24L01pPort.o NRF24L01p/port/RPI_WiringPi/NRF24L01pPort.cpp

${OBJECTDIR}/NRF24L01p/port/mbed/NRF24L01pPort.o: NRF24L01p/port/mbed/NRF24L01pPort.cpp
	${MKDIR} -p ${OBJECTDIR}/NRF24L01p/port/mbed
	${RM} "$@.d"
	$(COMPILE.cc) -g -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/NRF24L01p/port/mbed/NRF24L01pPort.o NRF24L01p/port/mbed/NRF24L01pPort.cpp

${OBJECTDIR}/NRF24L01p/port/stm32/NRF24L01pPort.o: NRF24L01p/port/stm32/NRF24L01pPort.cpp
	${MKDIR} -p ${OBJECTDIR}/NRF24L01p/port/stm32
	${RM} "$@.d"
	$(COMPILE.cc) -g -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/NRF24L01p/port/stm32/NRF24L01pPort.o NRF24L01p/port/stm32/NRF24L01pPort.cpp

${OBJECTDIR}/main.o: main.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/main.o main.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
