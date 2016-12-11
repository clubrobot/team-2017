USER = $(shell whoami)

PYTHON3 = $(shell basename $(shell readlink $(shell which python3)))
ifeq ($(USER),root)
PYTHON3_LIBRARIES_DIRECTORY	= $(shell $(PYTHON3) -c "import site; print(site.getsitepackages()[0])")
else
PYTHON3_LIBRARIES_DIRECTORY	= $(shell $(PYTHON3) -m site --user-site)
endif
PYTHON3_LIBRARY_NAME		= tcptalks

all:
	make python3_library

python3_library:
	mkdir -p $(PYTHON3_LIBRARIES_DIRECTORY)/$(PYTHON3_LIBRARY_NAME)
	cp *.py $(PYTHON3_LIBRARIES_DIRECTORY)/$(PYTHON3_LIBRARY_NAME)

.PHONY: all python3_library
