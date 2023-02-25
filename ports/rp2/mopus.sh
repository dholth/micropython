#!/bin/sh
make -j BOARD=PICO USER_C_MODULES=../../examples/opusmodule/micropython.cmake CFLAGS_EXTRA=-DMODULE_OGGZ_ENABLED=1,-DARDUINO=1,-DHAVE_CONFIG_H=1 V=1
