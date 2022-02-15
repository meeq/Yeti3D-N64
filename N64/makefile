PROG_NAME = Yeti3D_Pro

# Set V=1 to enable verbose Make output
ifneq ($(V),1)
.SILENT:
REDIRECT_STDOUT := >/dev/null
endif

# Set GITMODULES=0 to skip git submodule updates
GITMODULES ?= 1

# Set N64_INST to skip the really long toolchain build process.
ifeq ($(N64_INST),)
TOOLCHAIN_DIR := ./toolchain
# Ensure the toolchain directory exists in order to get its full path
override N64_INST := $(shell \
	mkdir -p "$(TOOLCHAIN_DIR)" && \
	realpath "$(TOOLCHAIN_DIR)" \
)
# Use a guard to cache the toolchain in-project
TOOLCHAIN_GUARD := $(TOOLCHAIN_DIR)/.installed
endif

# Toolchain directories
N64_INCLUDE_DIR := $(N64_INST)/mips64-elf/include
N64_LIB_DIR := $(N64_INST)/mips64-elf/lib
N64_GCC_PREFIX := $(N64_INST)/bin/mips64-elf-

# LibDragon tools
LIBDRAGON_DIR := ./libdragon
TOOLS_DIR := $(LIBDRAGON_DIR)/tools
CHKSUM64 := $(TOOLS_DIR)/chksum64
MKDFS := $(TOOLS_DIR)/mkdfs/mkdfs
N64TOOL := $(TOOLS_DIR)/n64tool

# LibDragon artifacts
LIBDRAGON_LIBS += $(LIBDRAGON_DIR)/libdragon.a
LIBDRAGON_LIBS += $(LIBDRAGON_DIR)/libdragonsys.a
ROM_HEADER := $(LIBDRAGON_DIR)/header

# Toolchain binaries
N64_CC := $(N64_GCC_PREFIX)gcc
N64_CXX := $(N64_GCC_PREFIX)g++
N64_AS := $(N64_GCC_PREFIX)as
N64_LD := $(N64_GCC_PREFIX)ld
N64_OBJCOPY := $(N64_GCC_PREFIX)objcopy

# Compiler flags
ASFLAGS = -march=vr4300 -mtune=vr4300 -Wa,--fatal-warnings
CFLAGS += -MMD -MP # Generate dependency files during compilation
CFLAGS += -march=vr4300 -mtune=vr4300 -std=gnu99 -Os -ffast-math
CFLAGS += -Wall -Wa,--fatal-warnings -fdiagnostics-color=always
CFLAGS += -falign-functions=32 -ffunction-sections -fdata-sections
CFLAGS += -I. -I.. -I../y3d -I"$(LIBDRAGON_DIR)/include" -I"$(N64_INCLUDE_DIR)"
CFLAGS += -DN64 -D__N64__
CFLAGS += -DSTBI_NO_HDR
CFLAGS += -DSHOW_FPS
CFLAGS += -DSHOW_SPLASH

# Linker flags
LDFLAGS += --library-path="$(LIBDRAGON_DIR)" --library-path="$(N64_LIB_DIR)"
LDFLAGS += --library=dragon --library=c --library=m --library=dragonsys
LDFLAGS += --script="./n64-exp.ld" -nostdlib --gc-sections --wrap __do_global_ctors

#
# Code artifacts
#

GAME_OBJS = \
	../game/ai/boost.o \
	../game/ai/bullet.o \
	../game/ai/death.o \
	../game/ai/explode.o \
	../game/ai/pickup.o \
	../game/ai/quad.o \
	../game/ai/sheba.o \
	../game/ai/static_model.o \
	../game/ai/static_sprite.o \
	../game/ai/teleporter.o \
	../game/ai/torch.o \
	../game/data.o \
	../game/entities.o \
	../game/game.o \
	../game/maps.o \
	../game/models.o \
	../game/screens.o \
	../game/sprites.o

YETI_OBJS =	\
	../y3d/y3d_animation.o \
	../y3d/y3d_ansic.o \
	../y3d/y3d_cell.o \
	../y3d/y3d_draw.o \
	../y3d/y3d_engine.o \
	../y3d/y3d_entity.o \
	../y3d/y3d_file.o \
	../y3d/y3d_fixed.o \
	../y3d/y3d_fruity.o \
	../y3d/y3d_image.o \
	../y3d/y3d_keyboard.o \
	../y3d/y3d_map.o \
	../y3d/y3d_matrix.o \
	../y3d/y3d_pixel.o \
	../y3d/y3d_record.o \
	../y3d/y3d_sound.o \
	../y3d/y3d_spanner.o \
	../y3d/y3d_surface.o \
	../y3d/y3d_vertex.o \
	../y3d/y3d_yeti.o

OBJS = main.o stb_image.o $(GAME_OBJS) $(YETI_OBJS)

#
# Compilation pipeline
#

all: toolchain libdragon libdragon-tools $(PROG_NAME).z64
.PHONY: all 

$(PROG_NAME).z64: $(PROG_NAME).bin $(PROG_NAME).dfs
$(PROG_NAME).z64: $(N64TOOL) $(CHKSUM64)
	@echo "    [Z64] $@"
	@rm -f "$@"
	$(N64TOOL) -o "$@" \
		--title "Yeti3D Pro" \
		--header "$(ROM_HEADER)" \
		"$(PROG_NAME).bin" \
		--offset 4M "$(PROG_NAME).dfs"
	$(CHKSUM64) "$@" $(REDIRECT_STDOUT)

$(PROG_NAME).dfs: $(MKDFS)
	@echo "    [DFS] $(notdir $@)"
	$(MKDFS) "$@" ./data/ $(REDIRECT_STDOUT)

$(PROG_NAME).bin: $(PROG_NAME).elf $(TOOLCHAIN_GUARD)
	@echo "    [BIN] $(notdir $@)"
	$(N64_OBJCOPY) -O binary "$<" "$@"

# Link using g++ for consistency with LibDragon's n64.mk
COMMA:=,
$(PROG_NAME).elf: $(OBJS) $(LIBDRAGON_LIBS)
	@echo "    [LD]  $(notdir $@)"
	$(N64_CXX) -o "$@" $^ -lc $(patsubst %,-Wl$(COMMA)%,$(LDFLAGS))

%.o: %.c $(TOOLCHAIN_GUARD)
	@echo "    [CC]  $<"
	$(N64_CC) -c $(CFLAGS) -o "$@" "$<"

%.o: %.s $(TOOLCHAIN_GUARD)
	@echo "    [AS]  $<"
	$(N64_AS) $(ASFLAGS) -o "$@" "$<"

#
# Housekeeping
#

clean:
	rm -f $(OBJS) $(patsubst %.o,%.d,$(OBJS)) *.z64 *.elf *.bin *.dfs
.PHONY: clean

distclean: clean toolchain-clean
	rm -Rf "$(LIBDRAGON_DIR)" .guard-*
	git restore "$(LIBDRAGON_DIR)"
.PHONY: distclean

#
# LibDragon submodule
#

$(LIBDRAGON_LIBS): | libdragon ;

$(CHKSUM64) $(MKDFS) $(N64TOOL): | libdragon-tools ;

libdragon: $(TOOLCHAIN_GUARD) | gitmodules
	@echo "    [DEP] $@"
	$(MAKE) -C "$(LIBDRAGON_DIR)" libdragon \
		V=$(V) N64_INST="$(N64_INST)" D=1 $(REDIRECT_STDOUT)
.PHONY: libdragon

libdragon-tools: | gitmodules
	@echo "    [DEP] $@"
	$(MAKE) -C "$(LIBDRAGON_DIR)" tools $(REDIRECT_STDOUT)
.PHONY: libdragon-tools

libdragon-clean:
	$(MAKE) -C "$(LIBDRAGON_DIR)" clean tools-clean $(REDIRECT_STDOUT)
.PHONY: libdragon-clean

#
# Compiler toolchain
#

ifdef TOOLCHAIN_GUARD
# Track the toolchain installation with a guard file
toolchain: $(TOOLCHAIN_GUARD)

# Copy the toolchain script from libdragon and run it
$(TOOLCHAIN_GUARD): | gitmodules
	@mkdir -p "$(TOOLCHAIN_DIR)/tmp"
	@echo "    [DEP] toolchain (this may take a while!)"
	cp "$(TOOLS_DIR)/build-toolchain.sh" "$(TOOLCHAIN_DIR)/tmp"
	export N64_INST="$(N64_INST)" && \
		cd "$(TOOLCHAIN_DIR)/tmp" && \
		bash build-toolchain.sh
	rm -Rf "$(TOOLCHAIN_DIR)/tmp"
	touch "$@"

toolchain-clean:
	rm -Rf "$(TOOLCHAIN_DIR)"
else
# NO-OP: toolchain is managed externally from the project
toolchain toolchain-clean: ;
endif
.PHONY: toolchain toolchain-clean

#
# Git submodules
#

ifeq ($(GITMODULES),1)
# Update git submodules if necessary
gitmodules:
	@if git submodule status | egrep -q '^[-]|^[+]' ; then \
		echo "    [GIT] submodules"; \
		git submodule update --init $(REDIRECT_STDOUT); \
		$(MAKE) libdragon-clean V=$(V) $(REDIRECT_STDOUT); \
    fi
else
gitmodules: ;
endif
.PHONY: gitmodules

# Include compiler-generated dependency files
-include $(DEPS)
