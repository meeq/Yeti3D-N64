/*
** Name: Yeti3D
** Desc: Portable GameBoy Advanced 3D Engine
** Auth: Derek J. Evans <derek@theteahouse.com.au>
**
** Copyright (C) 2003-2004 Derek J. Evans. All Rights Reserved.
**
** YY  YY EEEEEE TTTTTT IIIIII 33333  DDDDD
** YY  YY EE       TT     II       33 DD  DD
**  YYYY  EEEE     TT     II     333  DD  DD
**   YY   EE       TT     II       33 DD  DD
**   YY   EEEEEE   TT   IIIIII 33333  DDDDD
*/

#include "maps.h"

/******************************************************************************/

#define YETI_ROM

#ifndef __N64__
#include "maps/e1m1.h"
#include "maps/e1m2.h"
#include "maps/e1m3.h"
#include "maps/e1m4.h"
#include "maps/e1m5.h"
#include "maps/e1m6.h"
#include "maps/e1m7.h"
#include "maps/e1m8.h"
#include "maps/e1m9.h"

#include "maps/e2m1.h"
#include "maps/e2m2.h"
#include "maps/e2m3.h"
#include "maps/e2m4.h"
#include "maps/e2m5.h"
#include "maps/e2m6.h"
#include "maps/e2m7.h"
#include "maps/e2m8.h"
#include "maps/e2m9.h"

#include "maps/e3m1.h"
#include "maps/e3m2.h"
#include "maps/e3m3.h"
#include "maps/e3m4.h"
#include "maps/e3m5.h"
#include "maps/e3m6.h"
#include "maps/e3m7.h"
#include "maps/e3m8.h"
#include "maps/e3m9.h"

#include "maps/house.h"
#include "maps/race1.h"
#include "maps/race.h"
#include "maps/test.h"
#else
#include "maps/cave.h"
#endif
#ifndef __N64__
#include "maps/transparent.h"
#endif

rom_map_t* maps[] =
{
#ifndef __N64__
  &map_e1m1, &map_e1m2, &map_e1m3, &map_e1m4, &map_e1m5, &map_e1m6, &map_e1m7, &map_e1m8, &map_e1m9, &map_e2m1,
  &map_e2m2, &map_e2m3, &map_e2m4, &map_cave, &map_house, &map_race1, &map_race, &map_transparent, &map_e3m9
#else
  &map_cave
#endif
};
/******************************************************************************/

