#!/bin/bash

REPOSITORY=$(dirname $(readlink -f "$BASH_SOURCE"))
PROFILE="$HOME/.profile"

# Download and install Aduino IDE if it is not already installed

if [ -z "$ARDUINO_DIR" ]; then # ifndef ARDUINO_DIR
	
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
	echo export ARDUINO_DIR="/opt/$ARDUINO_SRC" >> "$PROFILE"

	# Launch Arduino IDE and give it time to settle in
	
	"/opt/$ARDUINO_SRC/arduino" &
	sleep 10
	pkill -n java

fi # ifndef ARDUINO_DIR

# Install Arduino-Makefile if it is not already installed

if [ -z "$ARDMK_DIR" ]; then # ifndef ARDMK_DIR
	sudo apt-get install arduino-mk
	echo export ARDMK_DIR="/usr/share/arduino" >> "$PROFILE"
fi

# Add the current user to the dialout group

sudo usermod -a -G dialout $USER

# Install udev rules

PYTHONPATH=$(python3 -m site --user-site)

UDEVRULES_DIRECTORY=/etc/udev/rules.d
UDEVRULE='KERNEL=="ttyUSB*", PROGRAM="/usr/bin/env PATH='"$PATH"' PYTHONPATH='"$PYTHONPATH"' '"$REPOSITORY/raspberrypi/robot getuuid"' /dev/%k", SYMLINK+="arduino/%c"'

echo $UDEVRULE | sudo tee "$UDEVRULES_DIRECTORY/serialtalks.rules" > /dev/null
sudo udevadm control --reload-rules

# Add the robot command to the user path if it is not alraedy in it

if [[ ":$PATH:" != *":$REPOSITORY/raspberrypi:"* ]]; then # if not **/team-2017/raspberrypi in PATH
	echo export PATH="$REPOSITORY/raspberrypi:\$PATH" >> "$PROFILE"
fi

# A reboot is required to source the .profile file

echo "My work here is done. Now it is time for me to retire."
echo "Please reboot your computer to take the changes into account"
