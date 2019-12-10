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
	${OBJECTDIR}/beelog/beelog.o \
	${OBJECTDIR}/beelog/bhdata.o \
	${OBJECTDIR}/beelog/curlftp.o \
	${OBJECTDIR}/beelog/epaper/WhiteFrame.o \
	${OBJECTDIR}/beelog/epaper/epd2in7.o \
	${OBJECTDIR}/beelog/epaper/epdif.o \
	${OBJECTDIR}/beelog/epaper/epdpaint.o \
	${OBJECTDIR}/beelog/epaper/font12.o \
	${OBJECTDIR}/beelog/epaper/font16.o \
	${OBJECTDIR}/beelog/epaper/font20.o \
	${OBJECTDIR}/beelog/epaper/font24.o \
	${OBJECTDIR}/beelog/epaper/font8.o \
	${OBJECTDIR}/beelog/epdwrapper.o \
	${OBJECTDIR}/beelog/get1wire.o \
	${OBJECTDIR}/beelog/getini.o \
	${OBJECTDIR}/beelog/getip.o \
	${OBJECTDIR}/beelog/hx711.o \
	${OBJECTDIR}/beelog/ini.o \
	${OBJECTDIR}/beelog/isr.o \
	${OBJECTDIR}/beelog/main.o \
	${OBJECTDIR}/beelog/raspimon.o \
	${OBJECTDIR}/beelog/readadc.o \
	${OBJECTDIR}/beelog/readads.o


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
LDLIBSOPTIONS=-L/usr/lib/arm-linux-gnueabihf -lcurl -lwiringPi -lm

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/beelog

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/beelog: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${LINK.cc} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/beelog ${OBJECTFILES} ${LDLIBSOPTIONS}

${OBJECTDIR}/beelog/beelog.o: beelog/beelog.c
	${MKDIR} -p ${OBJECTDIR}/beelog
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/beelog/beelog.o beelog/beelog.c

${OBJECTDIR}/beelog/bhdata.o: beelog/bhdata.c
	${MKDIR} -p ${OBJECTDIR}/beelog
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/beelog/bhdata.o beelog/bhdata.c

${OBJECTDIR}/beelog/curlftp.o: beelog/curlftp.c
	${MKDIR} -p ${OBJECTDIR}/beelog
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/beelog/curlftp.o beelog/curlftp.c

${OBJECTDIR}/beelog/epaper/WhiteFrame.o: beelog/epaper/WhiteFrame.c
	${MKDIR} -p ${OBJECTDIR}/beelog/epaper
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/beelog/epaper/WhiteFrame.o beelog/epaper/WhiteFrame.c

${OBJECTDIR}/beelog/epaper/epd2in7.o: beelog/epaper/epd2in7.cpp
	${MKDIR} -p ${OBJECTDIR}/beelog/epaper
	${RM} "$@.d"
	$(COMPILE.cc) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/beelog/epaper/epd2in7.o beelog/epaper/epd2in7.cpp

${OBJECTDIR}/beelog/epaper/epdif.o: beelog/epaper/epdif.cpp
	${MKDIR} -p ${OBJECTDIR}/beelog/epaper
	${RM} "$@.d"
	$(COMPILE.cc) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/beelog/epaper/epdif.o beelog/epaper/epdif.cpp

${OBJECTDIR}/beelog/epaper/epdpaint.o: beelog/epaper/epdpaint.cpp
	${MKDIR} -p ${OBJECTDIR}/beelog/epaper
	${RM} "$@.d"
	$(COMPILE.cc) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/beelog/epaper/epdpaint.o beelog/epaper/epdpaint.cpp

${OBJECTDIR}/beelog/epaper/font12.o: beelog/epaper/font12.c
	${MKDIR} -p ${OBJECTDIR}/beelog/epaper
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/beelog/epaper/font12.o beelog/epaper/font12.c

${OBJECTDIR}/beelog/epaper/font16.o: beelog/epaper/font16.c
	${MKDIR} -p ${OBJECTDIR}/beelog/epaper
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/beelog/epaper/font16.o beelog/epaper/font16.c

${OBJECTDIR}/beelog/epaper/font20.o: beelog/epaper/font20.c
	${MKDIR} -p ${OBJECTDIR}/beelog/epaper
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/beelog/epaper/font20.o beelog/epaper/font20.c

${OBJECTDIR}/beelog/epaper/font24.o: beelog/epaper/font24.c
	${MKDIR} -p ${OBJECTDIR}/beelog/epaper
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/beelog/epaper/font24.o beelog/epaper/font24.c

${OBJECTDIR}/beelog/epaper/font8.o: beelog/epaper/font8.c
	${MKDIR} -p ${OBJECTDIR}/beelog/epaper
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/beelog/epaper/font8.o beelog/epaper/font8.c

${OBJECTDIR}/beelog/epdwrapper.o: beelog/epdwrapper.cpp
	${MKDIR} -p ${OBJECTDIR}/beelog
	${RM} "$@.d"
	$(COMPILE.cc) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/beelog/epdwrapper.o beelog/epdwrapper.cpp

${OBJECTDIR}/beelog/get1wire.o: beelog/get1wire.c
	${MKDIR} -p ${OBJECTDIR}/beelog
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/beelog/get1wire.o beelog/get1wire.c

${OBJECTDIR}/beelog/getini.o: beelog/getini.c
	${MKDIR} -p ${OBJECTDIR}/beelog
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/beelog/getini.o beelog/getini.c

${OBJECTDIR}/beelog/getip.o: beelog/getip.c
	${MKDIR} -p ${OBJECTDIR}/beelog
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/beelog/getip.o beelog/getip.c

${OBJECTDIR}/beelog/hx711.o: beelog/hx711.c
	${MKDIR} -p ${OBJECTDIR}/beelog
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/beelog/hx711.o beelog/hx711.c

${OBJECTDIR}/beelog/ini.o: beelog/ini.c
	${MKDIR} -p ${OBJECTDIR}/beelog
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/beelog/ini.o beelog/ini.c

${OBJECTDIR}/beelog/isr.o: beelog/isr.c
	${MKDIR} -p ${OBJECTDIR}/beelog
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/beelog/isr.o beelog/isr.c

${OBJECTDIR}/beelog/main.o: beelog/main.c
	${MKDIR} -p ${OBJECTDIR}/beelog
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/beelog/main.o beelog/main.c

${OBJECTDIR}/beelog/raspimon.o: beelog/raspimon.c
	${MKDIR} -p ${OBJECTDIR}/beelog
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/beelog/raspimon.o beelog/raspimon.c

${OBJECTDIR}/beelog/readadc.o: beelog/readadc.c
	${MKDIR} -p ${OBJECTDIR}/beelog
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/beelog/readadc.o beelog/readadc.c

${OBJECTDIR}/beelog/readads.o: beelog/readads.c
	${MKDIR} -p ${OBJECTDIR}/beelog
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/beelog/readads.o beelog/readads.c

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
