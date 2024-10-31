#ifndef __ENEMY_H__
#define __ENEMY_H__

#include "entity.h"

Entity* generic_enemy_spawn(GFC_Vector3D position);

Entity* projectile_enemy_spawn(GFC_Vector3D position);

Entity* flying_enemy_spawn(GFC_Vector3D position);

#endif