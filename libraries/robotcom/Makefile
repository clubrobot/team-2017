ARDUINO_LIBRARY_FOLDER  = ~/Arduino/libraries
ARDUINO_LIBRARY_NAME    = RobotCom

install-all: install-raspberry install-arduino

install-arduino: robotcom.cpp robotcom.h
	mkdir -p $(ARDUINO_LIBRARY_FOLDER)/$(ARDUINO_LIBRARY_NAME)
	cp robotcom.cpp robotcom.h $(ARDUINO_LIBRARY_FOLDER)/$(ARDUINO_LIBRARY_NAME)

install-raspberry:
	# pip3 install pyserial
	echo "Not implemented yet"