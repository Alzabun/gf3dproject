#ifndef __PROJECTILE_H__
#define __PROJECTILE_H__

#include "entity.h"
#include "enemy.h"

// preserve direction depending on where the entity that's shooting is facing
Entity* projectile_spawn(GFC_Vector3D position, int direction);

Entity* bomb_spawn(GFC_Vector3D position);

Entity* kamehameha_spawn(GFC_Vector3D position, int direction);

#endif