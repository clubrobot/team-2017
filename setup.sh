#!/bin/bash

REPOSITORY=$(dirname $BASH_SOURCE)

# Download Arduino IDE

case $(uname -m) in
	i686	) OS=linux32;;
	x86_64	) OS=linux64;;
	arm*	) OS=linuxarm;;
	*		) exit 1;;
esac

URL=https://downloads.arduino.cc/arduino-1.6.12-$OS.tar.xz
wget $URL

# Install Arduino IDE

tar xf arduino-1.6.12-$OS.tar.xz
rm arduino-1.6.12-$OS.tar.xz
sudo mv arduino-1.6.12 /opt
echo export ARDUINO_DIR='/opt/arduino-1.6.12' >> $HOME/.bashrc

# Launch Arduino IDE and give it time to settle in

/opt/arduino-1.6.12/arduino &
sleep 10
pkill -n java

# Install Arduino-Makefile

sudo apt-get install arduino-mk
echo export ARDMK_DIR='/usr/share/arduino' >> $HOME/.bashrc

# Install Python libraries

pip3 install pyserial

# Install RobotCom components

cd $REPOSITORY/libraries/robotcom
make arduinolib
make pythonlib
sudo make udevrules
