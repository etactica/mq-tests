# new, simple simple makefile, hopefully suitable for both netbeans and openwrt sdk
# Should be able to "do the right thing" with include dirs and paths

EXECUTABLE=simplemq

# Other than main.c
SOURCES=

LDFLAGS+=-l:libmosquitto.so.1
	
ifeq ($(PLATFORM),owrt)
#bleh Even though we don't use -lssl ourselves... you need this when cross compiling
# Something about rpath and how it knows where to find things...
# There may be a better way, but this way _works_, let's get back to doing other things!
# Remember, this block is only when manually crosscompiling, openwrt's actual
# build chain "just works"
LDFLAGS+=-lssl -lcrypto
endif
	
# Shouldn't need to change anything under here...
CFLAGS+=-Wall -Wextra -std=gnu99
OBJECTS=$(SOURCES:.c=.o)
TEST_OBJECTS=$(TEST_SOURCES:.c=.o)
TEST_LOGS=$(TEST_SOURCES:.c=.c.xml)

all: env_exists $(SOURCES) $(EXECUTABLE)

help:
	@echo "Example usage:"
	@echo ""
	@echo "make clean all"
	@echo "   -> rebuild for linux"
	@echo "make PLATFORM=owrt clean all"
	@echo "   -> rebuild for OpenWRT (using default OWRT_STAGING directory)"
	@echo "make OWRT_STAGING=/some/path/to/staging PLATFORM=owrt clean all"
	@echo "   -> rebuild for OpenWRT using specified toolchain"
	@echo "   (TODO - build for owrt even if only toolchain is specifed)"

env_exists:
ifeq ($(PLATFORM),owrt)
	@echo "Building for OpenWRT"
ifeq ($(OWRT_STAGING),)
	@echo "Defaulting staging dir to $(DEF_OWRT_STAGING)"
OWRT_STAGING=$(DEF_OWRT_STAGING)
else
	@echo "Using OWRT_STAGING=$(OWRT_STAGING)"
endif
export STAGING_DIR=$(OWRT_STAGING)
# This should select the latest version, if you have more than one
TOOLS=$(lastword $(wildcard $(OWRT_STAGING)/toolchain-mips_gcc-*))
TTARGET=$(lastword $(wildcard $(OWRT_STAGING)/target-mips_*/usr))
LD=$(TOOLS)/bin/mips-openwrt-linux-uclibc-ld
CC=$(TOOLS)/bin/mips-openwrt-linux-uclibc-gcc
CFLAGS+=-Os -pipe -mips32 -mtune=mips32 -funit-at-a-time -fhonour-copts -msoft-float
CFLAGS+=$(EXTRA_CFLAGS)
CFLAGS+=-I$(TOOLS)/include -I$(TTARGET)/include
CFLAGS+=-DOWRT #-DIS_BACKFIRE=1
LINUX_DIR=$(lastword $(wildcard $(OWRT_STAGING)/../build_dir/linux-*/linux-2.6*))
CFLAGS+=-I$(LINUX_DIR)/include
LDFLAGS+=-L$(TOOLS)/lib -L$(TTARGET)/lib
else
# regular linux (and also, inside the OpenWRT build system)
	@echo "Building for linux"
CFLAGS+=$(EXTRA_CFLAGS)
LDFLAGS+=-L../../libs/dist$(shell getconf LONG_BIT)
#LDFLAGS+=-Wl,-rpath../../libs
endif

all: $(EXECUTABLE)
	@echo "rebuilding...."

$(EXECUTABLE): main.o $(OBJECTS) 
	$(CC) $< $(OBJECTS) $(LDFLAGS) -o $@

%o: %c
	$(CC) -c $(CFLAGS) $< -o $@
	
clean:
	rm -rf main.o
	rm -rf $(OBJECTS)
	rm -rf $(EXECUTABLE)

.PHONY: all clean env_exists help

