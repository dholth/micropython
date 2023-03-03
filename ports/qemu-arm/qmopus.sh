#!/bin/sh
make BOARD=microbit USER_C_MODULES=../../examples/opusmodule CFLAGS_EXTRA=-DMODULE_OGGZ_ENABLED=1 V=1