#!/bin/sh
make BOARD=ADAFRUIT_QTPY_RP2040 USER_C_MODULES=../../examples/opusmodule/micropython.cmake CFLAGS_EXTRA=-DMODULE_OPUS_ENABLED=1 V=1
