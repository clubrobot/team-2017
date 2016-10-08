USER = $(shell whoami)

ARDUINO_LIBRARIES_DIRECTORY	= $(shell grep -Po '(?<=sketchbook.path=).*' $(HOME)/.arduino15/preferences.txt)/libraries
ARDUINO_LIBRARIES_NAME		= RobotCom

PYTHON = $(shell readlink $(shell which python3))
ifeq ($(USER),root)
PYTHON_LIBRARIES_DIRECTORY	= /usr/local/lib/$(PYTHON)/dist-packages/
else
PYTHON_LIBRARIES_DIRECTORY	= $(shell $(PYTHON) -m site --user-site)
endif

EXECUTABLES_DIRECTORY		= /usr/local/bin

UDEV_RULES_DIRECTORY		= /etc/udev/rules.d

arduinolib: robotcom.cpp robotcom.h
	mkdir -p $(ARDUINO_LIBRARIES_DIRECTORY)/$(ARDUINO_LIBRARIES_NAME)
	cp robotcom.cpp robotcom.h $(ARDUINO_LIBRARIES_DIRECTORY)/$(ARDUINO_LIBRARIES_NAME)

pythonlib: robotcom.py
	mkdir -p $(PYTHON_LIBRARIES_DIRECTORY)
	cp robotcom.py $(PYTHON_LIBRARIES_DIRECTORY)

getuuid: getuuid
	cp getuuid $(EXECUTABLES_DIRECTORY)
	chmod +x $(EXECUTABLES_DIRECTORY)/getuuid

rules: robotcom.rules
	cp robotcom.rules $(UDEV_RULES_DIRECTORY)
	udevadm control --reload-rules

.PHONY: libarduino libpython getuuid rules
