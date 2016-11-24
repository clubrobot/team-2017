USER = $(shell whoami)

ARDUINO_SKETCHBOOK_PATH		= $(shell grep -Po '(?<=sketchbook.path=).*' $(HOME)/.arduino15/preferences.txt)
ARDUINO_LIBRARIES_DIRECTORY	= $(ARDUINO_SKETCHBOOK_PATH)/libraries
ARDUINO_LIBRARIES_NAME		= SerialTalks

PYTHON = $(shell basename $(shell readlink $(shell which python3)))
ifeq ($(USER),root)
PYTHON_LIBRARIES_DIRECTORY	= /usr/local/lib/$(PYTHON)/dist-packages/
else
PYTHON_LIBRARIES_DIRECTORY	= $(shell $(PYTHON) -m site --user-site)
endif

EXECUTABLES_DIRECTORY		= /usr/local/bin

UDEV_RULES_DIRECTORY		= /etc/udev/rules.d

arduino_library:
	mkdir -p $(ARDUINO_LIBRARIES_DIRECTORY)/$(ARDUINO_LIBRARIES_NAME)
	cp SerialTalks.cpp SerialTalks.h SerialUtils.h $(ARDUINO_LIBRARIES_DIRECTORY)/$(ARDUINO_LIBRARIES_NAME)

python3_library:
	mkdir -p $(PYTHON_LIBRARIES_DIRECTORY)
	cp serialtalks.py $(PYTHON_LIBRARIES_DIRECTORY)

getuuid: python3_library
	cp getuuid $(EXECUTABLES_DIRECTORY)
	chmod +x $(EXECUTABLES_DIRECTORY)/getuuid

udevrules: getuuid
	cp serialtalks.rules $(UDEV_RULES_DIRECTORY)
	udevadm control --reload-rules

.PHONY: arduino_library python3_library getuuid udevrules
