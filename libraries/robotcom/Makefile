ARDUINO_LIBRARIES_DIRECTORY	= ~/Arduino/libraries
ARDUINO_LIBRARIES_NAME		= RobotCom

PYTHON = python3
PYTHON_LIBRARIES_DIRECTORY	= $(shell $(PYTHON) -m site --user-site)

EXECUTABLES_DIRECTORY		= /usr/local/bin

UDEV_RULES_DIRECTORY		= /etc/udev/rules.d

install-all: install-raspberry install-arduino

install-arduino-library: robotcom.cpp robotcom.h
	mkdir -p $(ARDUINO_LIBRARIES_DIRECTORY)/$(ARDUINO_LIBRARIES_NAME)
	cp robotcom.cpp robotcom.h $(ARDUINO_LIBRARIES_DIRECTORY)/$(ARDUINO_LIBRARIES_NAME)

install-python-library: robotcom.py
	$(PYTHON) -m pip install pyserial
	mkdir -p $(PYTHON_LIBRARIES_DIRECTORY)
	cp robotcom.py $(PYTHON_LIBRARIES_DIRECTORY)

install-getuuid: getuuid
	cp getuuid $(EXECUTABLES_DIRECTORY)
	chmod +x $(EXECUTABLES_DIRECTORY)/getuuid

install-udev-rules: robotcom.rules
	cp robotcom.rules $(UDEV_RULES_DIRECTORY)
	udevadm control --reload-rules

.PHONY: install-all install-arduino install-raspberry install-raspberry-udev
