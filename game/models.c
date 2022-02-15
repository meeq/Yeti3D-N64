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

#define const

#include "models/model_rocket.h"
/*
#include "models/model_rifle.c"
#include "models/model_chicken.c"
#include "models/model_prabbit.c"
#include "models/model_tigger.c"
#include "models/model_potator.c"
#include "models/model_karrot.c"
#include "models/model_goblin.c"
#include "models/model_penguin.c"
#include "models/model_scarlet.c"
#include "models/model_banana.c"
*/
#include "models/model_rat.h"
#include "models/model_bauul.h"
#include "models/model_ogro.h"

#include "models/model_barrontree.h"
#include "models/model_leafyplant.h"
#include "models/model_cactus.h"
#include "models/model_potplant.h"

#include "models/model_pinechair.h"
#include "models/model_crate.h"

#include "models/model_overpass.h"
#include "models/model_bridge.h"
#include "models/model_pillar.h"
#include "models/model_emptypot.h"
#include "models/model_boost.h"
#include "models/model_quad.h"
#include "models/model_box.h"
#include "models/model_teleporter.h"
//#include "models/model_pm.c"
//#include "models/model_jeep.c"

#ifdef __N64__
int size_rocket = sizeof(model_rocket);
int size_rat = sizeof(model_rat);
int size_bauul = sizeof(model_bauul);
int size_ogro = sizeof(model_ogro);
int size_barrontree = sizeof(model_barrontree);
int size_leafyplant = sizeof(model_leafyplant);
int size_cactus = sizeof(model_cactus);
int size_potplant = sizeof(model_potplant);
int size_pinechair = sizeof(model_pinechair);
int size_crate = sizeof(model_crate);
int size_overpass = sizeof(model_overpass);
int size_bridge = sizeof(model_bridge);
int size_pillar = sizeof(model_pillar);
int size_emptypot = sizeof(model_emptypot);
int size_boost = sizeof(model_boost);
int size_quad = sizeof(model_quad);
int size_box = sizeof(model_box);
int size_teleporter = sizeof(model_teleporter);
#endif
