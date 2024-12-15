#ifndef __TERRAIN_H__
#define __TERRAIN_H__

#include "entity.h"

Entity* floor_terrain_spawn(GFC_Vector3D position);
Entity* small_terrain_spawn(GFC_Vector3D position);
Entity* bridge_terrain_spawn(GFC_Vector3D position);

Entity* test_spawn(GFC_Vector3D position);

#endif