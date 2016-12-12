SERIALTALKS_DIRECTORY = ./SerialTalks
TCPTALKS_DIRECTORY    = ./TCPTalks

all:
	make TCPTalks
	make SerialTalks

TCPTalks:
	make all -C $(TCPTALKS_DIRECTORY)

SerialTalks:
	make all -C $(SERIALTALKS_DIRECTORY)

.PHONY: all TCPTalks SerialTalks
