#ifndef __PROJECTILE_H__
#define __PROJECTILE_H__

#include "entity.h"
#include "enemy.h"
#include "SDL_mixer.h"
#include "gfc_audio.h"

// preserve direction depending on where the entity that's shooting is facing
Entity* projectile_spawn(GFC_Vector3D position, int direction);

Entity* bomb_spawn(GFC_Vector3D position);

Entity* kamehameha_spawn(GFC_Vector3D position, int direction, float charge);

Entity* bubble_spawn(GFC_Vector3D position);

Entity* laser_spawn(GFC_Vector3D position);


#endif