# --- nano ide 1.6

BOARD_TAG = nano
BOARD_SUB = atmega328

# Replace "$(shell ...)" by a constant tty symlink such as "/dev/arduino/<name>"
MONITOR_PORT = $(shell find /dev -name 'ttyUSB*' | head -n 1)

# Add all the libraries which are used in the sketch
# Notice the backslashes
ARDUINO_LIBS +=	EEPROM \
                SerialTalks

# This sketch specific: set the UUID
#UUID = rosita

# Arduino Makefile
ifdef UUID
CPPFLAGS		+= -DUUID=\"$(UUID)\"
endif
include $(ARDMK_DIR)/Arduino.mk
