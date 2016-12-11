USER = $(shell whoami)

ARDUINO_SKETCHBOOK_PATH		= $(shell grep -Po '(?<=sketchbook.path=).*' $(HOME)/.arduino15/preferences.txt)
ARDUINO_LIBRARIES_DIRECTORY	= $(ARDUINO_SKETCHBOOK_PATH)/libraries
ARDUINO_LIBRARY_NAME		= SerialTalks

PYTHON3 = $(shell basename $(shell readlink $(shell which python3)))
ifeq ($(USER),root)
PYTHON3_LIBRARIES_DIRECTORY	= $(shell $(PYTHON3) -c "import site; print(site.getsitepackages()[0])")
else
PYTHON3_LIBRARIES_DIRECTORY	= $(shell $(PYTHON3) -m site --user-site)
endif
PYTHON3_LIBRARY_NAME		= serialtalks

EXECUTABLES_DIRECTORY		= /usr/local/bin

UDEV_RULES_DIRECTORY		= /etc/udev/rules.d

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

udevrules: getuuid
	cp *.rules $(UDEV_RULES_DIRECTORY)
	udevadm control --reload-rules

.PHONY: all arduino_library python3_library getuuid udevrules
