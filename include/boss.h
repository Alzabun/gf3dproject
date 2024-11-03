#ifndef __BOSS_H__
#define __BOSS_H__

#include "entity.h"
#include "player.h"

Entity* boss_spawn(GFC_Vector3D position);
void getPlayer(playerData* player);

#endif