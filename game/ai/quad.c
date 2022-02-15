#include "../../y3d/y3d_yeti.h"
#include "../../y3d/y3d_engine.h"
#include "../../y3d/y3d_draw.h"
#include "../../y3d/y3d_quake.h"
#include "../../y3d/y3d_sound.h"
#include "../models.h"
#include "../sprites.h"
#include "../game.h"

void quad_tick(yeti_t* yeti, entity_t* e)
{
  pickup_tick(yeti, e, 170);
}

void quad_hit(yeti_t* yeti, entity_t* e1, entity_t* e2)
{
  if (e2->ontick == player_tick)
  {
    entity_destroy(e1);
    if (yeti->game.hud.ammo <= 205)
      yeti->game.hud.ammo += 50;
    else
      yeti->game.hud.ammo = 255; // max ammo
  }
}

void quad_init(yeti_t* yeti, entity_t* e)
{
  e->tt = i2f(8);
  e->radius = 128; // if 0, cannot be picked up
  e->visual.data = model_quad;
  e->visual.ondraw = yeti_model_draw;
  e->ontick = quad_tick;
  e->onhit  = quad_hit;
}
