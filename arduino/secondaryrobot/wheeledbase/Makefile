# Board properties
BOARD_UUID = wheeledbase
BOARD_TAG  = nano
BOARD_SUB  = atmega328

# Sketch sources
COMMON = ../../common
LOCAL_INO_SRCS = wheeledbase.ino
LOCAL_CPP_SRCS = \
	$(COMMON)/SerialTalks.cpp \
	$(COMMON)/DCMotor.cpp \
	$(COMMON)/RotaryEncoder.cpp \
	$(COMMON)/WheeledBase.cpp \
	$(COMMON)/Odometry.cpp \
	$(COMMON)/MyPID.cpp \
	$(COMMON)/Control.cpp

# Sketch libraries
ARDUINO_LIBS = EEPROM PID

# Congratulations! You made a pretty Makefile :)
# Now let the grown-ups do the hard work :D
MODULEMK_DIR = ../..
include $(MODULEMK_DIR)/Module.mk
