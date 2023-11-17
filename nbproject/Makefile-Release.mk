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
CC=g++
CCC=g++
CXX=g++
FC=gfortran
AS=as

# Macros
CND_PLATFORM=GNU-Linux
CND_DLIB_EXT=so
CND_CONF=Release
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
	${OBJECTDIR}/beelog/epaper/DEV_Config.o \
	${OBJECTDIR}/beelog/epaper/EPD_2in7.o \
	${OBJECTDIR}/beelog/epaper/EPD_2in7_test.o \
	${OBJECTDIR}/beelog/epaper/GUI_BMPfile.o \
	${OBJECTDIR}/beelog/epaper/GUI_Paint.o \
	${OBJECTDIR}/beelog/epaper/ImageData.o \
	${OBJECTDIR}/beelog/epaper/RPI_sysfs_gpio.o \
	${OBJECTDIR}/beelog/epaper/dev_hardware_SPI.o \
	${OBJECTDIR}/beelog/epaper/font12.o \
	${OBJECTDIR}/beelog/epaper/font12CN.o \
	${OBJECTDIR}/beelog/epaper/font16.o \
	${OBJECTDIR}/beelog/epaper/font20.o \
	${OBJECTDIR}/beelog/epaper/font24.o \
	${OBJECTDIR}/beelog/epaper/font24CN.o \
	${OBJECTDIR}/beelog/epaper/font8.o \
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
LDLIBSOPTIONS=-lcurl -lwiringPi -lbcm2835 -lm

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/beelog

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/beelog: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	g++ -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/beelog ${OBJECTFILES} ${LDLIBSOPTIONS}

${OBJECTDIR}/beelog/beelog.o: beelog/beelog.c
	${MKDIR} -p ${OBJECTDIR}/beelog
	${RM} "$@.d"
	$(COMPILE.c) -O2 -DRPI -DUSE_BCM2835_LIB -Ibeelog/epaper -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/beelog/beelog.o beelog/beelog.c

${OBJECTDIR}/beelog/bhdata.o: beelog/bhdata.c
	${MKDIR} -p ${OBJECTDIR}/beelog
	${RM} "$@.d"
	$(COMPILE.c) -O2 -DRPI -DUSE_BCM2835_LIB -Ibeelog/epaper -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/beelog/bhdata.o beelog/bhdata.c

${OBJECTDIR}/beelog/curlftp.o: beelog/curlftp.c
	${MKDIR} -p ${OBJECTDIR}/beelog
	${RM} "$@.d"
	$(COMPILE.c) -O2 -DRPI -DUSE_BCM2835_LIB -Ibeelog/epaper -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/beelog/curlftp.o beelog/curlftp.c

${OBJECTDIR}/beelog/epaper/DEV_Config.o: beelog/epaper/DEV_Config.c
	${MKDIR} -p ${OBJECTDIR}/beelog/epaper
	${RM} "$@.d"
	$(COMPILE.c) -O2 -DRPI -DUSE_BCM2835_LIB -Ibeelog/epaper -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/beelog/epaper/DEV_Config.o beelog/epaper/DEV_Config.c

${OBJECTDIR}/beelog/epaper/EPD_2in7.o: beelog/epaper/EPD_2in7.c
	${MKDIR} -p ${OBJECTDIR}/beelog/epaper
	${RM} "$@.d"
	$(COMPILE.c) -O2 -DRPI -DUSE_BCM2835_LIB -Ibeelog/epaper -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/beelog/epaper/EPD_2in7.o beelog/epaper/EPD_2in7.c

${OBJECTDIR}/beelog/epaper/EPD_2in7_test.o: beelog/epaper/EPD_2in7_test.c
	${MKDIR} -p ${OBJECTDIR}/beelog/epaper
	${RM} "$@.d"
	$(COMPILE.c) -O2 -DRPI -DUSE_BCM2835_LIB -Ibeelog/epaper -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/beelog/epaper/EPD_2in7_test.o beelog/epaper/EPD_2in7_test.c

${OBJECTDIR}/beelog/epaper/GUI_BMPfile.o: beelog/epaper/GUI_BMPfile.c
	${MKDIR} -p ${OBJECTDIR}/beelog/epaper
	${RM} "$@.d"
	$(COMPILE.c) -O2 -DRPI -DUSE_BCM2835_LIB -Ibeelog/epaper -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/beelog/epaper/GUI_BMPfile.o beelog/epaper/GUI_BMPfile.c

${OBJECTDIR}/beelog/epaper/GUI_Paint.o: beelog/epaper/GUI_Paint.c
	${MKDIR} -p ${OBJECTDIR}/beelog/epaper
	${RM} "$@.d"
	$(COMPILE.c) -O2 -DRPI -DUSE_BCM2835_LIB -Ibeelog/epaper -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/beelog/epaper/GUI_Paint.o beelog/epaper/GUI_Paint.c

${OBJECTDIR}/beelog/epaper/ImageData.o: beelog/epaper/ImageData.c
	${MKDIR} -p ${OBJECTDIR}/beelog/epaper
	${RM} "$@.d"
	$(COMPILE.c) -O2 -DRPI -DUSE_BCM2835_LIB -Ibeelog/epaper -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/beelog/epaper/ImageData.o beelog/epaper/ImageData.c

${OBJECTDIR}/beelog/epaper/RPI_sysfs_gpio.o: beelog/epaper/RPI_sysfs_gpio.c
	${MKDIR} -p ${OBJECTDIR}/beelog/epaper
	${RM} "$@.d"
	$(COMPILE.c) -O2 -DRPI -DUSE_BCM2835_LIB -Ibeelog/epaper -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/beelog/epaper/RPI_sysfs_gpio.o beelog/epaper/RPI_sysfs_gpio.c

${OBJECTDIR}/beelog/epaper/dev_hardware_SPI.o: beelog/epaper/dev_hardware_SPI.c
	${MKDIR} -p ${OBJECTDIR}/beelog/epaper
	${RM} "$@.d"
	$(COMPILE.c) -O2 -DRPI -DUSE_BCM2835_LIB -Ibeelog/epaper -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/beelog/epaper/dev_hardware_SPI.o beelog/epaper/dev_hardware_SPI.c

${OBJECTDIR}/beelog/epaper/font12.o: beelog/epaper/font12.c
	${MKDIR} -p ${OBJECTDIR}/beelog/epaper
	${RM} "$@.d"
	$(COMPILE.c) -O2 -DRPI -DUSE_BCM2835_LIB -Ibeelog/epaper -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/beelog/epaper/font12.o beelog/epaper/font12.c

${OBJECTDIR}/beelog/epaper/font12CN.o: beelog/epaper/font12CN.c
	${MKDIR} -p ${OBJECTDIR}/beelog/epaper
	${RM} "$@.d"
	$(COMPILE.c) -O2 -DRPI -DUSE_BCM2835_LIB -Ibeelog/epaper -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/beelog/epaper/font12CN.o beelog/epaper/font12CN.c

${OBJECTDIR}/beelog/epaper/font16.o: beelog/epaper/font16.c
	${MKDIR} -p ${OBJECTDIR}/beelog/epaper
	${RM} "$@.d"
	$(COMPILE.c) -O2 -DRPI -DUSE_BCM2835_LIB -Ibeelog/epaper -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/beelog/epaper/font16.o beelog/epaper/font16.c

${OBJECTDIR}/beelog/epaper/font20.o: beelog/epaper/font20.c
	${MKDIR} -p ${OBJECTDIR}/beelog/epaper
	${RM} "$@.d"
	$(COMPILE.c) -O2 -DRPI -DUSE_BCM2835_LIB -Ibeelog/epaper -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/beelog/epaper/font20.o beelog/epaper/font20.c

${OBJECTDIR}/beelog/epaper/font24.o: beelog/epaper/font24.c
	${MKDIR} -p ${OBJECTDIR}/beelog/epaper
	${RM} "$@.d"
	$(COMPILE.c) -O2 -DRPI -DUSE_BCM2835_LIB -Ibeelog/epaper -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/beelog/epaper/font24.o beelog/epaper/font24.c

${OBJECTDIR}/beelog/epaper/font24CN.o: beelog/epaper/font24CN.c
	${MKDIR} -p ${OBJECTDIR}/beelog/epaper
	${RM} "$@.d"
	$(COMPILE.c) -O2 -DRPI -DUSE_BCM2835_LIB -Ibeelog/epaper -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/beelog/epaper/font24CN.o beelog/epaper/font24CN.c

${OBJECTDIR}/beelog/epaper/font8.o: beelog/epaper/font8.c
	${MKDIR} -p ${OBJECTDIR}/beelog/epaper
	${RM} "$@.d"
	$(COMPILE.c) -O2 -DRPI -DUSE_BCM2835_LIB -Ibeelog/epaper -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/beelog/epaper/font8.o beelog/epaper/font8.c

${OBJECTDIR}/beelog/get1wire.o: beelog/get1wire.c
	${MKDIR} -p ${OBJECTDIR}/beelog
	${RM} "$@.d"
	$(COMPILE.c) -O2 -DRPI -DUSE_BCM2835_LIB -Ibeelog/epaper -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/beelog/get1wire.o beelog/get1wire.c

${OBJECTDIR}/beelog/getini.o: beelog/getini.c
	${MKDIR} -p ${OBJECTDIR}/beelog
	${RM} "$@.d"
	$(COMPILE.c) -O2 -DRPI -DUSE_BCM2835_LIB -Ibeelog/epaper -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/beelog/getini.o beelog/getini.c

${OBJECTDIR}/beelog/getip.o: beelog/getip.c
	${MKDIR} -p ${OBJECTDIR}/beelog
	${RM} "$@.d"
	$(COMPILE.c) -O2 -DRPI -DUSE_BCM2835_LIB -Ibeelog/epaper -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/beelog/getip.o beelog/getip.c

${OBJECTDIR}/beelog/hx711.o: beelog/hx711.c
	${MKDIR} -p ${OBJECTDIR}/beelog
	${RM} "$@.d"
	$(COMPILE.c) -O2 -DRPI -DUSE_BCM2835_LIB -Ibeelog/epaper -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/beelog/hx711.o beelog/hx711.c

${OBJECTDIR}/beelog/ini.o: beelog/ini.c
	${MKDIR} -p ${OBJECTDIR}/beelog
	${RM} "$@.d"
	$(COMPILE.c) -O2 -DRPI -DUSE_BCM2835_LIB -Ibeelog/epaper -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/beelog/ini.o beelog/ini.c

${OBJECTDIR}/beelog/isr.o: beelog/isr.c
	${MKDIR} -p ${OBJECTDIR}/beelog
	${RM} "$@.d"
	$(COMPILE.c) -O2 -DRPI -DUSE_BCM2835_LIB -Ibeelog/epaper -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/beelog/isr.o beelog/isr.c

${OBJECTDIR}/beelog/main.o: beelog/main.c
	${MKDIR} -p ${OBJECTDIR}/beelog
	${RM} "$@.d"
	$(COMPILE.c) -O2 -DRPI -DUSE_BCM2835_LIB -Ibeelog/epaper -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/beelog/main.o beelog/main.c

${OBJECTDIR}/beelog/raspimon.o: beelog/raspimon.c
	${MKDIR} -p ${OBJECTDIR}/beelog
	${RM} "$@.d"
	$(COMPILE.c) -O2 -DRPI -DUSE_BCM2835_LIB -Ibeelog/epaper -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/beelog/raspimon.o beelog/raspimon.c

${OBJECTDIR}/beelog/readadc.o: beelog/readadc.c
	${MKDIR} -p ${OBJECTDIR}/beelog
	${RM} "$@.d"
	$(COMPILE.c) -O2 -DRPI -DUSE_BCM2835_LIB -Ibeelog/epaper -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/beelog/readadc.o beelog/readadc.c

${OBJECTDIR}/beelog/readads.o: beelog/readads.c
	${MKDIR} -p ${OBJECTDIR}/beelog
	${RM} "$@.d"
	$(COMPILE.c) -O2 -DRPI -DUSE_BCM2835_LIB -Ibeelog/epaper -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/beelog/readads.o beelog/readads.c

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
