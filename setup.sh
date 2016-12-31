#!/bin/bash

REPOSITORY=$(dirname $(readlink -f "$BASH_SOURCE"))

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

# Install udev rules

UDEVRULES_DIRECTORY=/etc/udev/rules.d
UDEVRULES_NAME=serialtalks.rules
UDEVRULES_LINE='KERNEL=="ttyUSB*", PROGRAM="/usr/bin/env PATH='$PATH' PYTHONPATH='$(python3 -m site --user-site)' '"$REPOSITORY/raspberrypi/robot getuuid"' /dev/%k", SYMLINK+="arduino/%c"'

echo $UDEVRULES_LINE | sudo tee $UDEVRULES_DIRECTORY/$UDEVRULES_NAME > /dev/null
sudo udevadm control --reload-rules

# Add the robot command to the user path

echo export PATH="$REPOSITORY/raspberrypi:$PATH" >> "$HOME/.bashrc"
