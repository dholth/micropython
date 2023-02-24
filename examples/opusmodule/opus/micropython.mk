OPUS_MOD_DIR := $(USERMOD_DIR)

# Add all C files to SRC_USERMOD.
SRC_USERMOD_C += $(OPUS_MOD_DIR)/opusmodule.c $(OPUS_MOD_DIR)/oggzmodule.c

# Unix-installed opus, ogg, opusfile
LIBOPUS_CFLAGS := $(shell pkg-config --cflags opus)
LIBOPUS_LDFLAGS := $(shell pkg-config --libs opus)

LIBOGG_CFLAGS := $(shell pkg-config --cflags ogg)
LIBOGG_LDFLAGS := $(shell pkg-config --libs ogg)

LIBOPUSFILE_CFLAGS := $(shell pkg-config --cflags opusfile)
LIBOPUSFILE_LDFLAGS := $(shell pkg-config --libs opusfile)

LIBOGGZ_CFLAGS := $(shell pkg-config --cflags oggz)
LIBOGGZ_LDFLAGS := $(shell pkg-config --libs oggz)

CFLAGS_USERMOD += $(LIBOPUS_CFLAGS) $(LIBOGG_CFLAGS) $(LIBOPUSFILE_CFLAGS) $(LIBOGGZ_CFLAGS)
LDFLAGS_USERMOD += $(LIBOPUS_LDFLAGS) $(LIBOGG_LDFLAGS) $(LIBOPUSFILE_LDFLAGS) $(LIBOGGZ_LDFLAGS)

# We can add our module folder to include paths if needed
# This is not actually needed in this example.
CFLAGS_USERMOD += -I$(OPUS_MOD_DIR)
OPUS_MOD_DIR := $(USERMOD_DIR)
