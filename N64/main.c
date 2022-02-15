
#include "y3d/y3d_yeti.h"
#include "y3d/y3d_sound.h"
#include "y3d/y3d_image.h"
#include "game/maps.h"
#include "game/game.h"
#include "game/sprites.h"
#include "game/models.h"

#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <malloc.h>
#include <string.h>
#include <stdint.h>

#include <libdragon.h>

#define STBI_HEADER_FILE_ONLY
// the define above makes it only load the header portion
#include "stb_image.c"

/* hardware definitions */
// Pad buttons
#define A_BUTTON(a)     ((a) & 0x8000)
#define B_BUTTON(a)     ((a) & 0x4000)
#define Z_BUTTON(a)     ((a) & 0x2000)
#define START_BUTTON(a) ((a) & 0x1000)

// D-Pad
#define DU_BUTTON(a)    ((a) & 0x0800)
#define DD_BUTTON(a)    ((a) & 0x0400)
#define DL_BUTTON(a)    ((a) & 0x0200)
#define DR_BUTTON(a)    ((a) & 0x0100)

// Triggers
#define TL_BUTTON(a)    ((a) & 0x0020)
#define TR_BUTTON(a)    ((a) & 0x0010)

// Yellow C buttons
#define CU_BUTTON(a)    ((a) & 0x0008)
#define CD_BUTTON(a)    ((a) & 0x0004)
#define CL_BUTTON(a)    ((a) & 0x0002)
#define CR_BUTTON(a)    ((a) & 0x0001)

#define YETI_GAMMA (2.1)

extern void *__safe_buffer[3];

yeti_t yeti;
rom_map_t yetiMap;

#define NUM_MAPS 32
char *mapnames[NUM_MAPS] = {
    "rom://maps/e1m1.y3d",
    "rom://maps/e1m2.y3d",
    "rom://maps/e1m3.y3d",
    "rom://maps/e1m4.y3d",
    "rom://maps/e1m5.y3d",
    "rom://maps/e1m6.y3d",
    "rom://maps/e1m7.y3d",
    "rom://maps/e1m8.y3d",
    "rom://maps/e1m9.y3d",
    "rom://maps/e2m1.y3d",
    "rom://maps/e2m2.y3d",
    "rom://maps/e2m3.y3d",
    "rom://maps/e2m4.y3d",
    "rom://maps/e2m5.y3d",
    "rom://maps/e2m6.y3d",
    "rom://maps/e2m7.y3d",
    "rom://maps/e2m8.y3d",
    "rom://maps/e2m9.y3d",
    "rom://maps/e3m1.y3d",
    "rom://maps/e3m2.y3d",
    "rom://maps/e3m3.y3d",
    "rom://maps/e3m4.y3d",
    "rom://maps/e3m5.y3d",
    "rom://maps/e3m6.y3d",
    "rom://maps/e3m7.y3d",
    "rom://maps/e3m8.y3d",
    "rom://maps/e3m9.y3d",
    "rom://maps/house.y3d",
    "rom://maps/race1.y3d",
    "rom://maps/race.y3d",
    "rom://maps/test.y3d",
    "rom://maps/transparent.y3d"
};

int done = 0;

long long lastticks;
int frameticks;

int buf_size;
char *buf_ptr;

#define TV_TYPE (*(uint32_t *)0x80000300)
int tv_fps;

display_context_t dcon = 0;

unsigned short gButtons = 0;
struct controller_data gKeys;

volatile unsigned int gTicks;           /* incremented every vblank */

short int gCursorX;                     /* range is 0 to 63 (only 0 to 39 onscreen) */
short int gCursorY;                     /* range is 0 to 31 (only 0 to 27 onscreen) */

/* input - do getButtons() first, then getAnalogX() and/or getAnalogY() */
unsigned short getButtons(int pad)
{
    // Read current controller status
    memset(&gKeys, 0, sizeof(gKeys));
    controller_read(&gKeys);
    return (unsigned short)(gKeys.c[pad].data >> 16);
}

unsigned char getAnalogX(int pad)
{
    return (unsigned char)gKeys.c[pad].x;
}

unsigned char getAnalogY(int pad)
{
    return (unsigned char)gKeys.c[pad].y;
}

/* text functions */
void drawText(char *msg, int x, int y)
{
    if (dcon)
        graphics_draw_text(dcon, x, y, msg);
}

void printText(char *msg, int x, int y)
{
    if (x != -1)
        gCursorX = x;
    if (y != -1)
        gCursorY = y;

    if (dcon)
        graphics_draw_text(dcon, gCursorX*8, gCursorY*8, msg);

    gCursorY++;
    if (gCursorY > 29)
    {
        gCursorY = 0;
        gCursorX ++;
    }
}

/* vblank callback */
void vblCallback(void)
{
    gTicks++;
}

void delay(int cnt)
{
    int then = gTicks + cnt;
    while (then > gTicks) ;
}

/* debug helper function */
void debugText(char *msg, int x, int y, int d)
{
    while (!dcon)
        dcon = display_lock();
    display_show(dcon);
    dcon = 0;
    while (!dcon)
        dcon = display_lock();

    graphics_set_color(graphics_make_color(0xFF, 0xFF, 0xFF, 0xFF), graphics_make_color(0x00, 0x00, 0x00, 0xFF));
    printText(msg, x, y);

    // show display
    display_show(dcon);
    dcon = 0;
    while (!dcon)
        dcon = display_lock();

    delay(d);
}

void splash(void)
{
#ifdef SHOW_SPLASH
    unsigned char *image;
    unsigned short *dst;
    sprite_t *ss;
    int i, j, k, x, y, n;

    image = stbi_load("rom://splash.png", &x, &y, &n, 4);
    if (!image)
        return;

    ss = (sprite_t *)malloc(sizeof(sprite_t) + x * y * 2);
    ss->width = x;
    ss->height = y;
    ss->bitdepth = 2;
    ss->format = 0;
    ss->hslices = x / 32;
    ss->vslices = y / 16;
    dst = (unsigned short *)&ss->data[0];

    for (i=0; i<32; i++)
    {
        int xx, yy = 0;
        // fade in splash
        for (k=0; k<y; k++)
            for (j=0; j<x; j++)
            {
                if (i == 0)
                {
                    dst[k*x + j] = (unsigned short)1;
                }
                else if (i == 31)
                {
                    int r = image[(k*x + j)*4 + 0];
                    int g = image[(k*x + j)*4 + 1];
                    int b = image[(k*x + j)*4 + 2];
                    dst[k*x + j] = graphics_make_color(r, g, b, 0xFF) & 0x0000FFFF;
                }
                else
                {
                    int r = image[(k*x + j)*4 + 0] * i / 31;
                    int g = image[(k*x + j)*4 + 1] * i / 31;
                    int b = image[(k*x + j)*4 + 2] * i / 31;
                    dst[k*x + j] = graphics_make_color(r, g, b, 0xFF) & 0x0000FFFF;
                }
            }
        /* Invalidate data associated with sprite in cache */
        data_cache_hit_writeback_invalidate(ss->data, ss->width * ss->height * ss->bitdepth);
        while (!dcon)
            dcon = display_lock();
        //graphics_draw_sprite(dcon, 0, 0, ss);
        rdp_sync(SYNC_PIPE);
        rdp_set_default_clipping();
        rdp_enable_texture_copy();
        rdp_attach_display(dcon);
        // Draw image
        for (j=0; j<ss->vslices; j++)
        {
            xx = 0;
            for (k=0; k<ss->hslices; k++)
            {
                rdp_sync(SYNC_PIPE);
                rdp_load_texture_stride(0, 0, MIRROR_DISABLED, ss, j*ss->hslices + k);
                rdp_draw_sprite(0, xx, yy);
                xx += 32;
            }
            yy += 16;
        }
        rdp_detach_display();
        display_show(dcon);
        dcon = 0;
    }
    delay(tv_fps*2);
    for (i=31; i>=0; i--)
    {
        int xx, yy = 0;
        // fade out splash
        for (k=0; k<y; k++)
            for (j=0; j<x; j++)
            {
                if (i == 0)
                {
                    dst[k*x + j] = (unsigned short)1;
                }
                else if (i == 31)
                {
                    int r = image[(k*x + j)*4 + 0];
                    int g = image[(k*x + j)*4 + 1];
                    int b = image[(k*x + j)*4 + 2];
                    dst[k*x + j] = graphics_make_color(r, g, b, 0xFF) & 0x0000FFFF;
                }
                else
                {
                    int r = image[(k*x + j)*4 + 0] * i / 31;
                    int g = image[(k*x + j)*4 + 1] * i / 31;
                    int b = image[(k*x + j)*4 + 2] * i / 31;
                    dst[k*x + j] = graphics_make_color(r, g, b, 0xFF) & 0x0000FFFF;
                }
            }
        /* Invalidate data associated with sprite in cache */
        data_cache_hit_writeback_invalidate(ss->data, ss->width * ss->height * ss->bitdepth);
        while (!dcon)
            dcon = display_lock();
        //graphics_draw_sprite(dcon, 0, 0, ss);
        rdp_sync(SYNC_PIPE);
        rdp_set_default_clipping();
        rdp_enable_texture_copy();
        rdp_attach_display(dcon);
        // Draw image
        for (j=0; j<ss->vslices; j++)
        {
            xx = 0;
            for (k=0; k<ss->hslices; k++)
            {
                rdp_sync(SYNC_PIPE);
                rdp_load_texture_stride(0, 0, MIRROR_DISABLED, ss, j*ss->hslices + k);
                rdp_draw_sprite(0, xx, yy);
                xx += 32;
            }
            yy += 16;
        }
        rdp_detach_display();
        display_show(dcon);
        dcon = 0;
    }

    free(ss);
    stbi_image_free(image);
#endif
}

/* initialize console hardware */
void init_n64(void)
{
    /* enable interrupts (on the CPU) */
    init_interrupts();

    // Initialize display
    display_init(RESOLUTION_320x240, DEPTH_16_BPP, 2, GAMMA_NONE, ANTIALIAS_RESAMPLE);
    register_VI_handler(vblCallback);
    rdp_init();
    rdp_set_texture_flush(FLUSH_STRATEGY_NONE);

    if (TV_TYPE == 1)
        tv_fps = 50;
    else
        tv_fps = 60;
    frameticks = TIMER_TICKS(YETI_VIEWPORT_INTERVAL*1000);
    lastticks = timer_ticks();

    // Initialize controllers
    controller_init();

    // Initialize timer subsystem
    timer_init();

    // Initialize rom filesystem
    if (dfs_init(0xB0401000) != DFS_ESUCCESS)
    {
        while (!dcon)
            dcon = display_lock();
        graphics_draw_text(dcon, 9*8, 12*8, "ROM filesystem failed!");
        display_show(dcon);

        while (1) ;
    }

    // Initialize audio subsystem
    audio_init(22050, 2);
    buf_size = audio_get_buffer_length() * 4; // buffer length stereo samples
    buf_ptr = malloc(buf_size);

    splash();

    // switch to game resolution
    rdp_close();
    display_close();
    display_init(RESOLUTION_256x240, DEPTH_16_BPP, 3, GAMMA_NONE, ANTIALIAS_RESAMPLE);
    register_VI_handler(vblCallback);
    rdp_init();
    rdp_set_texture_flush(FLUSH_STRATEGY_NONE);
}

void Yeti_Cleanup()
{
    timer_close();
    audio_close();
    free(buf_ptr);
    rdp_close();
    display_close();
}

// read .y3d map
int Yeti_LoadMap(char *filename)
{
    int i;
    rom_cell_t *cell = &yetiMap.cells;
    int fd = open(filename, O_RDONLY);
    if (fd == -1)
    {
        while (!dcon)
            dcon = display_lock();
        graphics_draw_text(dcon, 7*8, 12*8, "Couldn't open map!");
        graphics_draw_text(dcon, 7*8, 13*8, filename);
        display_show(dcon);

        while (1) ;
    }

    read(fd, &yetiMap.name, 32);
    read(fd, &yetiMap.auth, 32);
    read(fd, &yetiMap.desc, 64);
    // now read in MAP_WIDTH*MAP_HEIGHT cells, byte swapping as needed
    for (i=0; i<(MAP_WIDTH*MAP_HEIGHT); i++)
    {
        u8 temp[2];
        read(fd, temp, 1);
        cell->swi = temp[0];
        read(fd, temp, 1);
        cell->ent = temp[0];
        read(fd, temp, 2);
        cell->top = temp[0] | temp[1]<<8;
        read(fd, temp, 1);
        cell->wtb = temp[0];
        read(fd, temp, 1);
        cell->sw2 = temp[0];
        read(fd, temp, 2);
        cell->bot = temp[0] | temp[1]<<8;
        read(fd, temp, 2);
        cell->lit = temp[0] | temp[1]<<8;
        read(fd, temp, 1);
        cell->wtt = temp[0];
        read(fd, temp, 1);
        cell->ttx = temp[0];
        read(fd, temp, 1);
        cell->wwi = temp[0];
        read(fd, temp, 1);
        cell->btx = temp[0];
        read(fd, temp, 1);
        cell->tos = temp[0];
        read(fd, temp, 1);
        cell->bos = temp[0];
        cell++;
    }
    close(fd);
    return 0;
}

void Yeti_Frame(void)
{
#ifdef SHOW_FPS
    static int prevTicks = 0;
    static int framecount = 0;

    framecount++;
    if ((gTicks - prevTicks) >= tv_fps)
    {
        yeti.fps = framecount;
        framecount = 0;
        prevTicks = gTicks;
    }
#endif

    //while((timer_ticks() - lastticks) < frameticks) ;
    lastticks = timer_ticks();

    game_loop(&yeti);

    if (dcon)
    {
        display_show(dcon);
        dcon = 0;
    }

    while (!dcon)
        dcon = display_lock();

    yeti.viewport.back = (pixel_buffer_t *)__safe_buffer[dcon-1];
}

// convert bgr555 data to rgba5551
void fix_sprites(void)
{
    for (int i=0; i<YETI_SPRITE_MAX; i++)
    {
        u16 *img = sprites[i];
        if (img)
        {
            for (int iy=0; iy<IMAGE_HEIGHT(img); iy++)
                for (int ix=0; ix<IMAGE_WIDTH(img); ix++)
                {
                    u16 c = IMAGE_PIXEL(img, ix, iy);
                    IMAGE_PIXEL(img, ix, iy) = RGB_SET((c>>0)&31, (c>>5)&31, (c>>10)&31);
                }
        }
    }

}

void fix_model(u16 *model, int size)
{
    int i, so;

    for (i=0; i<6; i++)
        model[i] = model[i]>>8 | model[i]<<8; // swap header bytes

    so = model[0] + model[1]*model[2]*3 + model[3]*2 + model[4]*6;

    // byte swap words from header to skin
    for (i=7; i<so; i++)
        model[i] = model[i]>>8 | model[i]<<8;

    // convert skin from BGR555 to RGBA551
    for (i=so; i<(size/2); i++)
    {
        u16 c = model[i]>>8 | model[i]<<8; // byte swap color
        model[i] = RGB_SET((c>>0)&31, (c>>5)&31, (c>>10)&31);
    }
}

void fix_models(void)
{
    fix_model((u16*)model_rocket, size_rocket);
    fix_model((u16*)model_rat, size_rat);
    fix_model((u16*)model_bauul, size_bauul);
    fix_model((u16*)model_ogro, size_ogro);
    fix_model((u16*)model_overpass, size_overpass);
    fix_model((u16*)model_bridge, size_bridge);
    fix_model((u16*)model_pillar, size_pillar);
    fix_model((u16*)model_emptypot, size_emptypot);
    fix_model((u16*)model_boost, size_boost);
    fix_model((u16*)model_quad, size_quad);
    fix_model((u16*)model_box, size_box);
    fix_model((u16*)model_teleporter, size_teleporter);
    fix_model((u16*)model_leafyplant, size_leafyplant);
    fix_model((u16*)model_barrontree, size_barrontree);
    fix_model((u16*)model_cactus, size_cactus);
    fix_model((u16*)model_potplant, size_potplant);
    fix_model((u16*)model_pinechair, size_pinechair);
    fix_model((u16*)model_crate, size_crate);
}

/* main code entry point */
int main(void)
{
    init_n64();

    fix_sprites();
    fix_models();

    while (!dcon)
        dcon = display_lock();

    yeti_init(&yeti, (u16 *)NULL, (u16 *)__safe_buffer[dcon-1], textures, palette);
    yeti_init_lua(&yeti, YETI_GAMMA);
    Yeti_Go_Episode();
    game_goto(&yeti.game, GAME_MODE_PLAY);
    player_init(yeti.player);

    while (!done)
    {
        Yeti_Frame();

        // update the keyboard structure - which pipes the events
        gButtons = getButtons(0);
        yeti.keyboard.state.up      = DU_BUTTON(gButtons) ? 1 : 0; // Up
        yeti.keyboard.state.down    = DD_BUTTON(gButtons) ? 1 : 0; // Down
        yeti.keyboard.state.left    = DL_BUTTON(gButtons) ? 1 : 0; // Left
        yeti.keyboard.state.right   = DR_BUTTON(gButtons) ? 1 : 0; // Right
        yeti.keyboard.state.l       = TL_BUTTON(gButtons) ? 1 : 0; // Strafe Left
        yeti.keyboard.state.r       = TR_BUTTON(gButtons) ? 1 : 0; // Strafe Right
        yeti.keyboard.state.a       = A_BUTTON(gButtons)  ? 1 : 0; // Fire
        yeti.keyboard.state.b       = B_BUTTON(gButtons)  ? 1 : 0; // Jump
        yeti.keyboard.state.select  = Z_BUTTON(gButtons)  ? 1 : 0; //
        yeti.keyboard.state.start   = START_BUTTON(gButtons) ? 1 : 0; // Menu
        yeti.keyboard.state.rotate  = 0; // Camera
        yeti.keyboard.state.cu      = CU_BUTTON(gButtons) ? 1 : 0; // Look Up
        yeti.keyboard.state.cd      = CD_BUTTON(gButtons) ? 1 : 0; // Look Down
        yeti.keyboard.state.cl      = CL_BUTTON(gButtons) ? 1 : 0; // Prev Weapon
        yeti.keyboard.state.cr      = CR_BUTTON(gButtons) ? 1 : 0; // Next Weapon
        yeti.keyboard.state.mouse.x = getAnalogX(0);
        yeti.keyboard.state.mouse.y = getAnalogY(0);

    }

    return 0;
}

void Yeti_Go_Episode(void)
{
    if (yeti.game.episode < NUM_MAPS)
    {
        Yeti_LoadMap(mapnames[yeti.game.episode]);
        game_load_map(&yeti, &yetiMap);
    }
    else
    {
        // levels embedded in game
        game_load_map(&yeti, maps[yeti.game.episode - NUM_MAPS]);
    }
}

/******************************************************************************/

void sound_play(int id)
{
}

void sound_loop(int id)
{
}

void sound_stop(int id)
{
}

/******************************************************************************/
