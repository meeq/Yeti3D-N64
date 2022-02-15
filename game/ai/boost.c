#include "../../y3d/y3d_yeti.h"
#include "../../y3d/y3d_engine.h"
#include "../models.h"
#include "../game.h"

void boost_tick(yeti_t* yeti, entity_t* e)
{
  pickup_tick(yeti, e, 256);
}

void boost_hit(yeti_t* yeti, entity_t* e1, entity_t* e2)
{
  if (e2->ontick == player_tick)
  {
    entity_destroy(e1);
    if (yeti->game.hud.health <= 205)
      yeti->game.hud.health += 50;
    else
      yeti->game.hud.health = 255; // max health
  }
}

void boost_init(yeti_t* yeti, entity_t* e)
{
  e->tt = i2f(8);
  e->radius = 128; // if 0, cannot be picked up
  e->visual.data = model_boost;
  e->visual.ondraw = yeti_model_draw;
  e->ontick = boost_tick;
  e->onhit  = boost_hit;
}
