#!/bin/bash

REPOSITORY=$(dirname "$BASH_SOURCE")

# Download Arduino IDE

case $(uname -m) in
	i686	) OS=linux32;;
	x86_64	) OS=linux64;;
	arm*	) OS=linuxarm;;
	*		) exit 1;;
esac

ARDUINO_URL=https://downloads.arduino.cc/arduino-1.6.12-$OS.tar.xz
ARDUINO_TAR=$(basename "$ARDUINO_URL")
wget "$ARDUINO_URL" -O "$ARDUINO_TAR"

# Install Arduino IDE

ARDUINO_SRC=$(tar -tf "$ARDUINO_TAR" | head -1)
ARDUINO_SRC=${ARDUINO_SRC%/} # Remove trailing '/'
tar -xvf "$ARDUINO_TAR"
rm "$ARDUINO_TAR"
sudo mv "$ARDUINO_SRC" /opt
echo export ARDUINO_DIR="/opt/$ARDUINO_SRC" >> "$HOME/.bashrc"

# Launch Arduino IDE and give it time to settle in

"/opt/$ARDUINO_SRC/arduino" &
sleep 10
pkill -n java

# Install Arduino-Makefile

sudo apt-get install arduino-mk
echo export ARDMK_DIR="/usr/share/arduino" >> "$HOME/.bashrc"

# Add the current user to the dialout group

sudo usermod -a -G dialout $USER

# Install Python libraries

pip3 install pyserial

# Install RobotCom components

cd "$REPOSITORY/libraries/robotcom"
make arduinolib
make pythonlib
sudo make udevrules
