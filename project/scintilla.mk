# save vars for later
CFLAGS_ENV  := $(CFLAGS)
CCFLAGS_ENV := $(CCFLAGS)
ASFLAGS_ENV := $(ASFLAGS)
LDFLAGS_ENV := $(LDFLAGS)

#$(error "PATH: $(PATH))
THISDIR_FROMTOP := $(patsubst $(dir $(ROOT_DIR))%,%,$(CURDIR))
$(info "THISDIR_FROMTOP=$(THISDIR_FROMTOP)")
TOP :=
$(foreach W,$(subst /, ,$(THISDIR_FROMTOP)),$(eval TOP:=$(TOP)../))
#TOP=../
$(info "TOP=$(TOP)")

CCTOOL = g++
CTOOL = gcc

TARGET_NAME = libscintilla
ifeq ($(OS),Windows_NT)
TARGET_NAME := $(TARGET_NAME).dll
else
TARGET_NAME := $(TARGET_NAME).so
endif

SOURCES := \
	$(wildcard $(TOP)scintilla/src/*.cxx) \
	$(wildcard $(TOP)scintilla/fox/*.cxx) \
	$(wildcard $(TOP)scintilla/lexlib/*.cxx) \
	$(wildcard $(TOP)scintilla/lexers/*.cxx)

INCLUDES    := \
	-I$(TOP)scintilla/src \
	-I$(TOP)scintilla/include \
	-I$(TOP)scintilla/lexlib \
	-I$(TOP)fox/include \
	-Isystem/mingw/include \
	-Isystem/boost/include \

SCINTILLA_DEFINES = -DFOX -DSCI_LEXER
# -std=c++11 
CCFLAGS :=  -Wall -Wextra -Woverloaded-virtual -Wshadow -fPIC $(SCINTILLA_DEFINES)
CFLAGS  := -Wall -fPIC -pedantic -std=c99 $(SCINTILLA_DEFINES)
LDFLAGS := -shared -Wl,-L. -Wl,--enable-auto-image-base

ifeq ($(_DEBUG),1)
CCFLAGS += -g -O0 -DDEBUG -D_DEBUG
CFLAGS  += -g -O0 -DDEBUG -D_DEBUG
LDFLAGS += -g
else    
CCFLAGS += -Os -DNDEBUG
CFLAGS  += -Os -DNDEBUG
LDFLAGS +=
endif

CFLAGS  += $(CFLAGS_ENV)
CCFLAGS += $(CCFLAGS_ENV)
ASFLAGS += $(ASFLAGS_ENV)
LDFLAGS += $(LDFLAGS_ENV)

LIBS = -lFOX-1.7.dll

# let make find sources in VPATH as prerequisites for pattern rules
SOURCES      := $(patsubst $(TOP)%,%,$(SOURCES))
OBJECTS      := $(SOURCES:.cxx=.o)
DEPENDENCIES := $(SOURCES:.cxx=.depend)
$(info "SOURCES=$(SOURCES)")
$(info "OBJECTS=$(OBJECTS)")
$(info "DEPENDENCIES=$(DEPENDENCIES)")

all Release Debug: $(TARGET_NAME)

### IMPLICIT RULES ###

# search path for source files and other prerequisites
VPATH=$(TOP)

%.depend : %.cxx
	@echo "Extracting dependencies $@"
	mkdir -p '$(dir $@)'
	$(CCTOOL) $< -MM -MF $@ -MT '$(addsuffix .o,$(basename $@))' $(INCLUDES) $(CCFLAGS)

%.o : %.cxx %.depend
	@echo "Compiling $@"
	$(CCTOOL) $(INCLUDES) $(CCFLAGS) -o $@ -c $<

### BUILD TARGETS ###

ifeq ($(OS),Windows_NT)
IMPLIB_SWITCHES := -Wl,--out-implib,$(TARGET_NAME).a
endif

$(TARGET_NAME): $(OBJECTS)
	# echo "$(OBJECTS)"
	$(CCTOOL) $(LDFLAGS) -o $(TARGET_NAME) $(OBJECTS) $(LIBS) $(IMPLIB_SWITCHES)
#	mkdir -p $(OUTPUT_DBG)
#	objcopy --only-keep-debug $(TARGET_NAME) $(TARGET_BIN_DBG)
#	objcopy --strip-debug $(TARGET_NAME)
#	objcopy --add-gnu-debuglink=$(TARGET_BIN_DBG) $(TARGET_NAME)

clean:
	rm -rf $(OUTPUT) Makefile.bak

-include $(DEPENDENCIES)
