//#include "gp2x.h"
#ifndef GP2X
#include "y3d/y3d_keyboard.h"
#endif
#include "y3d/y3d_yeti.h"
#include "game/maps.h"
#include "game/game.h"
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <SDL/SDL.h>
//#include "flush_uppermem_cache.h"   // Squidgehack

#define YETI_GAMMA (2.1)

typedef u16 rgb555_t;
typedef struct
{
  rgb555_t pixels[YETI_VIEWPORT_HEIGHT][YETI_VIEWPORT_WIDTH];
} framebuffer_t;

framebuffer_t framebuffer; //gives us somewhere to draw our stuff.
yeti_t yeti;
int oldTick = 0;		// Used to help limit the framerate.
int done = 0;
SDL_Surface *screen;

SDL_Event event;

void Yeti_Cleanup();

static pixel_converter_t pixel_converter;
rom_map_t yetiMap;

void sdl3d_flip()
{
	int currentTick;
	int waitTicks;

	currentTick=SDL_GetTicks();

	oldTick += YETI_VIEWPORT_INTERVAL;
	waitTicks = (oldTick + (YETI_VIEWPORT_INTERVAL)) - currentTick;
	oldTick = currentTick;

	/* Only loop through the game loop and draw if enough
	   time has passed since the last frame.
	   The viewport_to_video function takes the frame we've drawm to
	   our 15-bpp 'framebuffer' in memory, converts it to 16-bit,
	   and throws it onto our actual video surface.  Look at yeti.c
	   to see what is going on under the hood here. */
	//if(waitTicks>0)
	//{
	//	SDL_Delay(waitTicks);
	//}

	game_loop(&yeti);

    pixel_buffer_draw(yeti.viewport.back, (rgb555_t*)screen->pixels, screen->pitch, &pixel_converter);

	//viewport_to_video( (rgb555_t*)screen->pixels, screen->pitch, &yeti.viewport, 0xf800, 0x07e0, 0x001f);
#ifdef GP2X
    //flush_uppermem_cache(&screen->pixels, &screen->pixels + 320*240, 0);     // Squidgehack
#endif
	SDL_Flip(screen);
}

// read .y3d map
int loadMap(const char *filename)
{
    FILE *f;
    f = fopen(filename, "rb");
    if(f)
    {
        int n = fread(&yetiMap, sizeof(yetiMap), 1, f);
        return 0;
    }
    return -1;
}

int main(int argc, char *argv[])
{
    #ifdef GP2X
	//system("/sbin/rmmod mmuhack");         // Squidgehack
    //system("/sbin/insmod mmuhack.o");
   // ramHack();
    #endif

	//Initialize SDL video.
	if ( SDL_Init(SDL_INIT_VIDEO|SDL_INIT_TIMER|SDL_INIT_JOYSTICK) < 0 )
	{
		printf("Unable to init SDL: %s\n", SDL_GetError());
		exit(1);
	}
	atexit(Yeti_Cleanup);

	// Squidgehack
	#ifdef GP2X
    /*int mmufd = open("/dev/mmuhack", O_RDWR);
    if(mmufd < 0)
    {
        printf("MMU hack failed");
    }
    else
    {
        printf("MMU hack loaded");
        close(mmufd);
    }*/
    #endif
	// Squidgehack --^

	SDL_Joystick* joystick = SDL_JoystickOpen(0);

	screen = SDL_SetVideoMode(YETI_VIEWPORT_WIDTH, YETI_VIEWPORT_HEIGHT, 16, SDL_HWSURFACE);
	if(screen == NULL)
	{
		printf("Unable to set %d, %d 16-bit video: %s\n",YETI_VIEWPORT_WIDTH,
               YETI_VIEWPORT_HEIGHT, SDL_GetError());
		exit(1);
	}
	SDL_ShowCursor(0);
	SDL_JoystickEventState(SDL_ENABLE);

    yeti_init(&yeti, &framebuffer, &framebuffer, textures, palette);
    yeti_init_lua(&yeti, YETI_GAMMA);
    game_load_map(&yeti, maps[0]);
    game_goto(&yeti.game, GAME_MODE_PLAY);
    player_init(yeti.player);

    if(!pixel_converter.length)
    {
        pixel_converter_init(&pixel_converter, 31 << 11, 31 << 6, 31 << 0, 0);
    }

    int mNumber = 1;
	while(done==0)
	{
		sdl3d_flip();
		while(SDL_PollEvent(&event))
		{
			if(event.type == SDL_QUIT){ done = 1;}
			if(event.type == SDL_KEYDOWN)
			{
				if ( event.key.keysym.sym == SDLK_ESCAPE ) { done = 1; }  // Exit
				else if(event.key.keysym.sym == SDLK_SPACE) // Change Map
				{
				    if(mNumber < 19) // load internal maps
                        game_load_map(&yeti, maps[mNumber]);
                    else // load external .y3d
                        game_load_map(&yeti, &yetiMap);
				    mNumber++;
				    // external load
				    if(mNumber == 19)
                        loadMap("maps/wacked.y3d");
				    else if(mNumber == 20)
                        loadMap("maps/car.y3d");
				    else if(mNumber == 21)
                        loadMap("maps/church.y3d");
				    else if(mNumber == 22)
                        loadMap("maps/gpldemo.y3d");
				    else if(mNumber == 23)
                        loadMap("maps/new.y3d");
				    else if(mNumber == 24)
                        loadMap("maps/sand.y3d");
				    else if(mNumber == 25)
                        loadMap("maps/terrain.y3d");
				    else if(mNumber == 26)
                        loadMap("maps/test0.y3d");
				    else if(mNumber == 27)
                        loadMap("maps/test1.y3d");
				    else if(mNumber == 28)
                        loadMap("maps/test2.y3d");
				    else if(mNumber == 29)
                        loadMap("maps/test3.y3d");
				    else if(mNumber == 30)
                        loadMap("maps/test4.y3d");
				    else if(mNumber == 31)
                        loadMap("maps/test5.y3d");
				    else if(mNumber == 32)
                        loadMap("maps/thelab.y3d");
				    else if(mNumber == 33)
                        loadMap("maps/thq.y3d");
                    else if(mNumber == 34)
                        mNumber = 0;
                }
			}
		}

		// update the keyboard structure - which pipes the events
		#ifdef DINGUX
		Uint8 *keystate = SDL_GetKeyState(NULL);
		yeti.keyboard.state.up      = keystate[SDLK_UP]; // Up
		yeti.keyboard.state.down    = keystate[SDLK_DOWN]; // Down
        	yeti.keyboard.state.left    = keystate[SDLK_LEFT]; // Left
        	yeti.keyboard.state.right   = keystate[SDLK_RIGHT]; // Right
        	yeti.keyboard.state.l       = keystate[SDLK_TAB];; // Look up
        	yeti.keyboard.state.r       = keystate[SDLK_BACKSPACE];; // Look down
		yeti.keyboard.state.a       = keystate[SDLK_LCTRL]; // Fire
		yeti.keyboard.state.b       = keystate[SDLK_LALT];; // Jump
		yeti.keyboard.state.select  = keystate[SDLK_RETURN]; // Menu
		yeti.keyboard.state.start   = keystate[SDLK_UNKNOWN]; // ????
		yeti.keyboard.state.rotate  = keystate[SDLK_LSHIFT]; // Camera
		#else
		/* For Motorola MAGX 
		int mX, mY;
		SDL_GetMouseState(&mX, &mY);*/
		Uint8 *keystate = SDL_GetKeyState(NULL);
		yeti.keyboard.state.up      = keystate[SDLK_UP]; // Up
		yeti.keyboard.state.down    = keystate[SDLK_DOWN]; // Down
        	yeti.keyboard.state.left    = keystate[SDLK_LEFT]; // Left
        	yeti.keyboard.state.right   = keystate[SDLK_RIGHT]; // Right
        	yeti.keyboard.state.l       = keystate[SDLK_PLUS];; // Look up
        	yeti.keyboard.state.r       = keystate[SDLK_MINUS];; // Look down
		yeti.keyboard.state.a       = keystate[SDLK_RETURN]; // Fire
		yeti.keyboard.state.b       = keystate[SDLK_PAUSE];; // Jump
		yeti.keyboard.state.select  = keystate[SDLK_2]; // Menu
		yeti.keyboard.state.start   = keystate[SDLK_1]; // ????
		yeti.keyboard.state.rotate  = keystate[SDLK_5]; // Camera
		/*yeti.keyboard.state.mouse.x = keystate[SDLK_6];
		yeti.keyboard.state.mouse.y = keystate[SDLK_4];
		yeti.keyboard.y      = keystate[SDLK_6];      
		yeti.keyboard.x      = keystate[SDLK_4];*/
		#endif
	}

	return 0;
}

// Lineus - handy way to clean up... called at exit of program
// place all cleanup code here - SDL_Quit(), etc...
void Yeti_Cleanup()
{
    SDL_Quit();
    #ifdef GP2X
   /* system("/sbin/rmmod mmuhack");  // unload Squidgehack
    system("exec /usr/gp2x/gp2xmenu");  // start menu*/
    #endif
}
