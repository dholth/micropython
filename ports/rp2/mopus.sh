#!/bin/sh
make -j BOARD=PICO USER_C_MODULES=../../examples/opusmodule/micropython.cmake CFLAGS_EXTRA=-DMODULE_OPUS_ENABLED=1,-DMODULE_OGGZ_ENABLED=1 V=1
