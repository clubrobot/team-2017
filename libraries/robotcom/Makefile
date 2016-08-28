ARDUINO_LIBRARY_FOLDER	= ~/Arduino/libraries
ARDUINO_LIBRARY_NAME	= RobotCom

PYTHON = python3
PYTHON_LIBRARY_FOLDER = $(shell $(PYTHON) -m site --user-site)

pyserial: $(PYTHON_LIBRARY_FOLDER)/serial
	@echo $(PYTHON_LIBRARY_FOLDER)/serial
	$(PYTHON) -m pip install pyserial

install-all: install-raspberry install-arduino

install-arduino: robotcom.cpp robotcom.h
	mkdir -p $(ARDUINO_LIBRARY_FOLDER)/$(ARDUINO_LIBRARY_NAME)
	cp robotcom.cpp robotcom.h $(ARDUINO_LIBRARY_FOLDER)/$(ARDUINO_LIBRARY_NAME)

install-raspberry: pyserial robotcom.py
	cp robotcom.py $(PYTHON_LIBRARY_FOLDER)

.PHONY: install-all install-arduino install-raspberry