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

TARGET_NAME = proxivision
ifeq ($(OS),Windows_NT)
TARGET_NAME := $(TARGET_NAME).exe
endif

SOURCES := $(wildcard $(TOP)proxivision/*.cxx)

INCLUDES    := \
	-I$(TOP)proxivision \
	-I$(TOP)fox/include \
	-I$(TOP)scintilla/include \
	-I$(TOP)picojson \
	-I$(TOP)xdgmime/src \
	-I/mingw64/include/libxml2
INCLUDESDEP := \
	$(INCLUDES) \
	-isystem/mingw/include \
	-isystem/boost/include

CCFLAGS := -Wall -Wextra -Woverloaded-virtual -Wshadow -fPIC
# -DUNICODE -D_UNICODE -DWIN32 -D_WIN32 -D_WINDOWS
CFLAGS  := -Wall -fPIC -pedantic -std=c99
LDFLAGS := -Wl,-L.

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

LIBS = \
	-lscintilla \
	-lFOX-1.7 \
	-lxml2 \
	-lurlmon \
	-lole32 \
	-lgdi32

CCTOOL = g++
CTOOL = gcc

RESICONS = \
	proxivision/res/proxivision.o \
	proxivision/res/document-new.o \
	proxivision/res/document-open.o \
	proxivision/res/document-save.o \
	proxivision/res/document-save-as.o \
	proxivision/res/document-save-all.o \
	proxivision/res/edit-cut.o \
	proxivision/res/edit-copy.o \
	proxivision/res/edit-delete.o \
	proxivision/res/edit-paste.o \
	proxivision/res/edit-redo.o \
	proxivision/res/edit-undo.o \
	proxivision/res/format-shift-left.o \
	proxivision/res/format-shift-right.o \
	proxivision/res/search.o \
	proxivision/res/search-next.o \
	proxivision/res/search-prev.o \
	proxivision/res/go-backward.o \
	proxivision/res/go-forward.o \
	proxivision/res/fonts.o \
	free-file-icons/16px/_blank.o \
	free-file-icons/16px/_page.o \
	free-file-icons/16px/aac.o \
	free-file-icons/16px/ai.o \
	free-file-icons/16px/aiff.o \
	free-file-icons/16px/avi.o \
	free-file-icons/16px/bmp.o \
	free-file-icons/16px/c.o \
	free-file-icons/16px/cpp.o \
	free-file-icons/16px/css.o \
	free-file-icons/16px/csv.o \
	free-file-icons/16px/dat.o \
	free-file-icons/16px/dmg.o \
	free-file-icons/16px/doc.o \
	free-file-icons/16px/dotx.o \
	free-file-icons/16px/dwg.o \
	free-file-icons/16px/dxf.o \
	free-file-icons/16px/eps.o \
	free-file-icons/16px/exe.o \
	free-file-icons/16px/flv.o \
	free-file-icons/16px/gif.o \
	free-file-icons/16px/h.o \
	free-file-icons/16px/hpp.o \
	free-file-icons/16px/html.o \
	free-file-icons/16px/ics.o \
	free-file-icons/16px/iso.o \
	free-file-icons/16px/java.o \
	free-file-icons/16px/jpg.o \
	free-file-icons/16px/js.o \
	free-file-icons/16px/key.o \
	free-file-icons/16px/less.o \
	free-file-icons/16px/mid.o \
	free-file-icons/16px/mp3.o \
	free-file-icons/16px/mp4.o \
	free-file-icons/16px/mpg.o \
	free-file-icons/16px/odf.o \
	free-file-icons/16px/ods.o \
	free-file-icons/16px/odt.o \
	free-file-icons/16px/otp.o \
	free-file-icons/16px/ots.o \
	free-file-icons/16px/ott.o \
	free-file-icons/16px/pdf.o \
	free-file-icons/16px/php.o \
	free-file-icons/16px/png.o \
	free-file-icons/16px/ppt.o \
	free-file-icons/16px/psd.o \
	free-file-icons/16px/py.o \
	free-file-icons/16px/qt.o \
	free-file-icons/16px/rar.o \
	free-file-icons/16px/rb.o \
	free-file-icons/16px/rtf.o \
	free-file-icons/16px/sass.o \
	free-file-icons/16px/scss.o \
	free-file-icons/16px/sql.o \
	free-file-icons/16px/tga.o \
	free-file-icons/16px/tgz.o \
	free-file-icons/16px/tiff.o \
	free-file-icons/16px/txt.o \
	free-file-icons/16px/wav.o \
	free-file-icons/16px/xls.o \
	free-file-icons/16px/xlsx.o \
	free-file-icons/16px/xml.o \
	free-file-icons/16px/yml.o \
	free-file-icons/16px/zip.o

# let make find sources in VPATH as prerequisites for pattern rules
SOURCES      := $(patsubst $(TOP)%,%,$(SOURCES))
OBJECTS      := $(SOURCES:.cxx=.o) $(RESICONS)
DEPENDENCIES := $(SOURCES:.cxx=.depend)
$(info "SOURCES=$(SOURCES)")
$(info "OBJECTS=$(OBJECTS)")
$(info "DEPENDENCIES=$(DEPENDENCIES)")

all Release Debug: $(TARGET_NAME)

### IMPLICIT RULES ###

# search path for source files and other prerequisites
VPATH=$(TOP)

define xxd_full
$(subst -,_,$(subst /,_,$(subst .,_,$<)))
endef

define xxd_short
icon_$(subst -,_,$(subst .,_,$(notdir $(basename $1))))_ptr
endef

# unsigned char ___proxivision_proxivision_png[] = {
%.o: %.png
	@echo "PNG Compiling $@"
	mkdir -p '$(dir $@)'
	xxd -i "$<" '$(addsuffix .cxx,$(basename $@))'
	echo "unsigned char const* $(call xxd_short,$<) = $(call xxd_full,$<);" >> '$(addsuffix .cxx,$(basename $@))'
	$(CCTOOL) $(INCLUDES) $(CCFLAGS) -o $@ -c '$(addsuffix .cxx,$(basename $@))'

%.o: %.gif
	@echo "GIF Compiling $@"
	mkdir -p '$(dir $@)'
	xxd -i "$<" '$(addsuffix .cxx,$(basename $@))'
	echo "unsigned char const* $(call xxd_short,$<) = $(call xxd_full,$<);" >> '$(addsuffix .cxx,$(basename $@))'
	$(CCTOOL) $(INCLUDES) $(CCFLAGS) -o $@ -c '$(addsuffix .cxx,$(basename $@))'

%.depend : %.c
	@echo "Extracting dependencies $@"
	mkdir -p '$(dir $@)'
	$(CTOOL) $< -MM -MF $@ -MT '$(addsuffix .o,$(basename $@))' $(INCLUDESDEP) $(CFLAGS)

%.depend : %.cxx
	@echo "Extracting dependencies $@"
	mkdir -p '$(dir $@)'
	$(CCTOOL) $< -MM -MF $@ -MT '$(addsuffix .o,$(basename $@))' $(INCLUDESDEP) $(CCFLAGS)

%.o : %.c %.depend
	@echo "Compiling $@"
	$(CTOOL) $(INCLUDES) $(CFLAGS) -o $@ -c $<

%.o : %.cxx %.depend
	@echo "Compiling $@"
	$(CCTOOL) $(INCLUDES) $(CCFLAGS) -o $@ -c $<

### BUILD TARGETS ###
./%.dll: /mingw64/bin/%.dll
	cp -f "$<" "$@"

./%.json: proxivision/ScintillaLangStyles.json
	cp -f "$<" "$@"

$(TARGET_NAME): $(OBJECTS) ./libstdc++-6.dll ./libxml2-2.dll ./libgcc_s_seh-1.dll ./libbz2-1.dll \
	./libjpeg-8.dll ./libpng16-16.dll ./libtiff-5.dll ./zlib1.dll ./libiconv-2.dll ./liblzma-5.dll \
	./libwinpthread-1.dll ./libiconv-2.dll ./libzstd.dll ./ScintillaLangStyles.json
	# echo "$(OBJECTS)" 

	#which $(CCTOOL)
	$(CCTOOL) $(LDFLAGS) -o $(TARGET_NAME) $(OBJECTS) $(LIBS)

#	mkdir -p $(OUTPUT_DBG)
#	objcopy --only-keep-debug $(TARGET_NAME) $(TARGET_BIN_DBG)
#	objcopy --strip-debug $(TARGET_NAME)
#	objcopy --add-gnu-debuglink=$(TARGET_BIN_DBG) $(TARGET_NAME)

clean:
	rm -rf $(OUTPUT) Makefile.bak

-include $(DEPENDENCIES)
