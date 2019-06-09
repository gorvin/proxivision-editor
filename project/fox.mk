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

TARGET_NAME = libFOX-1.7
ifeq ($(OS),Windows_NT)
TARGET_NAME := $(TARGET_NAME).dll
else
TARGET_NAME := $(TARGET_NAME).so
endif

SOURCES := $(wildcard $(TOP)fox/lib/*.cxx) $(wildcard $(TOP)fox/lib/*.cpp)

INCLUDES    := \
	-I$(TOP)fox/lib \
	-I$(TOP)fox/include \
	-Isystem/mingw/include \
	-Isystem/boost/include \

FOX_DEFINES= \
	-DPACKAGE_NAME="fox" \
	-DPACKAGE_TARNAME="fox" \
	-DPACKAGE_VERSION="1.7.65"  \
	-DPACKAGE_STRING="fox 1.7.65" \
	-DPACKAGE_BUGREPORT="jeroen@fox-toolkit.com" \
	-DPACKAGE_URL="" \
	-DPACKAGE="fox" \
	-DVERSION="1.7.65" \
	-DSTDC_HEADERS=1 \
	-DHAVE_SYS_TYPES_H=1 \
	-DHAVE_SYS_STAT_H=1 \
	-DHAVE_STDLIB_H=1 \
	-DHAVE_STRING_H=1 \
	-DHAVE_MEMORY_H=1 \
	-DHAVE_STRINGS_H=1 \
	-DHAVE_INTTYPES_H=1 \
	-DHAVE_STDINT_H=1 \
	-DHAVE_UNISTD_H=1 \
	-DLT_OBJDIR=".libs/" \
	-D__EXTENSIONS__=1 \
	-D_ALL_SOURCE=1 \
	-D_GNU_SOURCE=1 \
	-D_POSIX_PTHREAD_SEMANTICS=1 \
	-D_TANDEM_SOURCE=1 \
	-DX_DISPLAY_MISSING=1 \
	-DTIME_WITH_SYS_TIME=1 \
	-DHAVE_DIRENT_H=1 \
	-DHAVE_LIBPTHREAD=1 \
	-DHAVE_SEMAPHORE_H=1 \
	-DHAVE_IMMINTRIN_H=1 \
	-DFOXDLL \
	-DFOXDLL_EXPORTS \
	-DWIN32 \
	-D_WINDOWS \
	-DUNICODE \
	-D_UNICODE \
	-D_USRDLL \
	-D_CRT_SECURE_NO_WARNINGS \
	-DHAVE_JPEG_H=1 \
	-DHAVE_PNG_H=1 \
	-DHAVE_TIFF_H=1 \
	-DHAVE_ZLIB_H=1 \
	-DHAVE_BZLIB_H=1 \
	-DHAVE_BZ2LIB_H=1 \
	-DNO_XIM \
	-DHAVE_GL_H=1 \
	-DHAVE_GLU_H=1 
# -std=c++11 
CCFLAGS :=  -Wall -Wextra -Woverloaded-virtual -Wshadow -fPIC $(FOX_DEFINES) 
# -DUNICODE -D_UNICODE -DWIN32 -D_WIN32 -D_WINDOWS
CFLAGS  := -Wall -fPIC -pedantic -std=c99 $(FOX_DEFINES)
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

LIBS = -lstdc++ \
	-lmingw32 \
	-lgcc_s \
	-lgcc \
	-lmoldname \
	-lmingwex \
	-lmsvcrt \
	-lpthread \
	-ladvapi32 \
	-lshell32 \
	-luser32 \
	-lkernel32 \
	-lgdi32 \
	-limm32 \
	-lws2_32 \
	-lopengl32 \
	-lwinspool \
	-ljpeg \
	-lpng \
	-ltiff \
	-lbz2 \
	-lz

# let make find sources in VPATH as prerequisites for pattern rules
SOURCES      := $(patsubst $(TOP)%,%,$(SOURCES))
OBJECTS      := $(SOURCES:.cpp=.o)
DEPENDENCIES := $(SOURCES:.cpp=.depend)
$(info "SOURCES=$(SOURCES)")
$(info "OBJECTS=$(OBJECTS)")
$(info "DEPENDENCIES=$(DEPENDENCIES)")

#.SUFFIXES: .cxx .cpp .c

all Release Debug: $(TARGET_NAME)

### IMPLICIT RULES ###

# search path for source files and other prerequisites
VPATH=$(TOP)

%.depend : %.cpp
	@echo "Extracting dependencies $@"
	mkdir -p '$(dir $@)'
	$(CCTOOL) $< -MM -MF $@ -MT '$(addsuffix .o,$(basename $@))' $(INCLUDES) $(CCFLAGS)

%.o : %.cpp %.depend
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
