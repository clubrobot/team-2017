# Arduino Board properties

BOARD_TAG = nano
BOARD_SUB = atmega328
BOARD_UUID = alfred

# Add all the libraries which are used in the sketch (separated by spaces)

ARDUINO_LIBS +=	

# Arduino Makefile (do not edit the following lines)

## Add SerialTalks libraries
ARDUINO_LIBS +=	EEPROM SerialTalks

## Define the BOARD_UUID preprocessor variable
ifdef BOARD_UUID
CPPFLAGS += -DBOARD_UUID=\"$(BOARD_UUID)\"
endif

## Guess MONITOR_PORT if not specified
ifndef MONITOR_PORT
MONITOR_PORT := /dev/arduino/$(BOARD_UUID)
ifeq ($(shell test -e "$(MONITOR_PORT)"; echo $$?),1)
MONITOR_PORT := $(shell find /dev -name "ttyUSB*")
ifneq ($(shell echo $(MONITOR_PORT) | wc -w),1)
MONITOR_PORT := "$(MONITOR_PORT)"
endif
endif
endif

## Call the Arduino Makefile
include $(ARDMK_DIR)/Arduino.mk
