#!/bin/sh
make -j USER_C_MODULES=../../examples/opusmodule CFLAGS_EXTRA=-DMODULE_OPUS_ENABLED=1  V=1
