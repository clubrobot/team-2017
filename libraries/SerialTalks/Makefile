MAKEFILE_DIRECTORY = $(dir $(realpath $(firstword $(MAKEFILE_LIST))))

ifeq ($(shell id -u),0)
USER = root
else
USER = $(shell whoami)
endif

ARDUINO_SKETCHBOOK_PATH		= $(shell grep -Po '(?<=sketchbook.path=).*' $(HOME)/.arduino15/preferences.txt)
ARDUINO_LIBRARIES_DIRECTORY	= $(ARDUINO_SKETCHBOOK_PATH)/libraries
ARDUINO_LIBRARY_NAME		= SerialTalks

PYTHON3 = $(shell readlink $$(which python3))
ifeq ($(USER),root)
PYTHON3_LIBRARIES_DIRECTORY	= $(shell $(PYTHON3) -c "import site; print(site.getsitepackages()[0])")
else
PYTHON3_LIBRARIES_DIRECTORY	= $(shell $(PYTHON3) -m site --user-site)
endif
PYTHON3_LIBRARY_NAME		= serialtalks

EXECUTABLES_DIRECTORY		= /usr/local/bin

UDEVRULES_DIRECTORY		= /etc/udev/rules.d
UDEVRULES_NAME			= serialtalks.rules
UDEVRULES_LINE			= KERNEL=="ttyUSB*", PROGRAM="/usr/bin/env PYTHONPATH=$(PYTHON3_LIBRARIES_DIRECTORY) $(PYTHON3) $(MAKEFILE_DIRECTORY)/getuuid /dev/%k", SYMLINK+="arduino/%c"

all:
	make arduino_library
	make python3_library
	sudo make getuuid
	sudo make getlogs
	sudo make udevrules

arduino_library:
	mkdir -p $(ARDUINO_LIBRARIES_DIRECTORY)/$(ARDUINO_LIBRARY_NAME)
	cp arduino/*.cpp arduino/*.h $(ARDUINO_LIBRARIES_DIRECTORY)/$(ARDUINO_LIBRARY_NAME)
	rm -f $(ARDUINO_LIBRARIES_DIRECTORY)/$(ARDUINO_LIBRARY_NAME)/unit.cpp

python3_library:
	mkdir -p $(PYTHON3_LIBRARIES_DIRECTORY)/$(PYTHON3_LIBRARY_NAME)
	cp python3/*.py $(PYTHON3_LIBRARIES_DIRECTORY)/$(PYTHON3_LIBRARY_NAME)

getuuid: python3_library
	cp "$@" $(EXECUTABLES_DIRECTORY)
	chmod +x $(EXECUTABLES_DIRECTORY)/"$@"

getlogs: python3_library
	cp "$@" $(EXECUTABLES_DIRECTORY)
	chmod +x $(EXECUTABLES_DIRECTORY)/"$@"

udevrules:
	echo '$(UDEVRULES_LINE)' | sudo tee $(UDEVRULES_DIRECTORY)/$(UDEVRULES_NAME) > /dev/null
	sudo udevadm control --reload-rules

.PHONY: all arduino_library python3_library getuuid udevrules
