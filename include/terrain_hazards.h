#ifndef __TERRAIN_HAZ_H__
#define __TERRAIN_HAZ_H__

#include "entity.h"

Entity* water_spawn_32(GFC_Vector3D position);
Entity* sand_spawn_32(GFC_Vector3D position);
Entity* ice_spawn_32(GFC_Vector3D position);
Entity* lava_spawn_32(GFC_Vector3D position);

#endif