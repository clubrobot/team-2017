BOARD_UUID = alfred
BOARD_TAG  = nano
BOARD_SUB  = atmega328

# Sketch sources
COMMON = ../../common
LOCAL_INO_SRCS = test.ino
LOCAL_CPP_SRCS = \
	instructions.cpp \
	$(COMMON)/SerialTalks.cpp \
	$(COMMON)/UltrasonicSensor.cpp \
	$(COMMON)/Clock.cpp \


# Sketch libraries
ARDUINO_LIBS = EEPROM Servo

# Congratulations! You made a pretty Makefile :)
# Now let the grown-ups do the hard work :D
MODULEMK_DIR = ../..
include $(MODULEMK_DIR)/Module.mk
