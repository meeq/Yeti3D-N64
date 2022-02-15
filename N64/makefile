ifdef $(N64_INST)
ROOTDIR = $(N64_INST)
else
ROOTDIR = /opt/toolchains/n64
endif

HEADERDIR = $(ROOTDIR)/headers
HEADER = $(HEADERDIR)/header

TOOLDIR = $(ROOTDIR)/bin
CHKSUM64 = $(TOOLDIR)/chksum64
MKDFS = $(TOOLDIR)/mkdfs
N64TOOL = $(TOOLDIR)/n64tool

LDSCRIPTSDIR = $(ROOTDIR)/ldscripts
LINK_FLAGS = -T$(LDSCRIPTSDIR)/n64-exp.ld -Map map.txt -nostdlib

LIBPATH = -L$(ROOTDIR)/mips64-elf/lib -L$(ROOTDIR)/mips64-elf/lib/gcc/mips64-elf/4.6.2 -L$(ROOTDIR)/mips64-elf/mips64-elf/lib
INCPATH = -I./ -I../ -I../y3d/ -I$(ROOTDIR)/mips64-elf/include -I$(ROOTDIR)/mips64-elf/mips64-elf/include
CCFLAGS = -std=gnu99 -march=vr4300 -mtune=vr4300 -G 0 -Ofast -Wall -c $(INCPATH)
CCFLAGS += -DSTBI_NO_HDR
CCFLAGS += -D__N64__
CCFLAGS += -DSHOW_FPS
CCFLAGS += -DSHOW_SPLASH
HWCCFLAGS = -std=gnu99 -march=vr4300 -mtune=vr4300 -O1 -G 0 -Wall $(INCPATH)
ASFLAGS = -mtune=vr4300 -march=vr4300

N64PREFIX = $(ROOTDIR)/mips64-elf/bin/mips64-elf-
CC = $(N64PREFIX)gcc
AS = $(N64PREFIX)as
LD = $(N64PREFIX)ld
OBJCOPY = $(N64PREFIX)objcopy

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

PROG_NAME = Yeti3D_Pro
OBJS = main.o stb_image.o $(GAME_OBJS) $(YETI_OBJS)
LIBS = -lm -ldragon -lc -ldragonsys -lnosys

$(PROG_NAME).z64: $(PROG_NAME).dfs $(PROG_NAME).elf
	$(OBJCOPY) $(PROG_NAME).elf $(PROG_NAME).bin -O binary
	rm -f $(PROG_NAME).v64
	$(N64TOOL) -l 8192K -t "Yeti3D Pro" -h $(HEADER) -o $(PROG_NAME).z64 $(PROG_NAME).bin -s 4M $(PROG_NAME).dfs
	$(CHKSUM64) $(PROG_NAME).z64

$(PROG_NAME).elf : $(OBJS)
	$(LD) -o $(PROG_NAME).elf $(OBJS) $(LINK_FLAGS) $(LIBPATH) $(LIBS)

%.o: %.c
	$(CC) $(CCFLAGS) $< -o $@

%.o: %.s
	$(AS) $(ASFLAGS) $< -o $@

$(PROG_NAME).dfs :
	$(MKDFS) $(PROG_NAME).dfs ./data/

all: $(PROG_NAME).z64

clean:
	rm -f *.o ../game/*.o ../game/ai/*.o ../y3d/*.o *.z64 *.elf *.bin *.dfs
