$(info "PWD=$(CURDIR)")

THISPATH := $(abspath $(lastword $(MAKEFILE_LIST)))
THISDIR := $(dir $(THISPATH))
export PATH    := /mingw64/bin:/usr/bin:/bin:/usr/local/bin:$(THISDIR)tools:$(PATH)
export CFLAGS  := $(CFLAGS)
export CCFLAGS := $(CCFLAGS)
export ASFLAGS := $(ASFLAGS)
export LDFLAGS := $(LDFLAGS)

ifeq ($(V), 1)
export V=1
endif

ifeq ($(_DEBUG), 1)
export _DEBUG=1
endif

.PHONY: all fox scintilla proxivision clean-all clean-fox clean-scintilla clean-proxivision

all: fox

fox:
	mkdir -p bld
	$(MAKE) -C bld -f $(THISDIR)project/fox.mk ROOT_DIR=$(THISDIR)

scintilla:
	mkdir -p bld
	$(MAKE) -C bld -f $(THISDIR)project/scintilla.mk ROOT_DIR=$(THISDIR)

proxivision: 
	mkdir -p bld
	$(MAKE) -C bld -f $(THISDIR)project/proxivision.mk ROOT_DIR=$(THISDIR)
	
clean-all:
	rm -rf bld/ Makefile.bak
	
clean-fox:
	rm -rf bld/fox/ bld/libFOX*

clean-scintilla:
	rm -rf bld/scintilla/ bld/libscintilla*

clean-proxivision:
	rm -rf bld/proxivision/ bld/proxivision.exe
