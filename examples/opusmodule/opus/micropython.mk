OPUS_MOD_DIR := $(USERMOD_DIR)

# Add all C files to SRC_USERMOD.
SRC_USERMOD_C += $(OPUS_MOD_DIR)/oggzmodule.c

ARDUINO_LIBOPUS_DIR = $(TOP)/../arduino-libopus
include $(ARDUINO_LIBOPUS_DIR)/micropython.mk

CFLAGS_USERMOD += -DARDUINO -DHAVE_CONFIG_H -I$(ARDUINO_LIBOPUS_DIR)/src -Wno-unused-but-set-variable -Wno-unused-function -fsingle-precision-constant -Dabort=abort_
