#ifndef __TERRAIN_H__
#define __TERRAIN_H__

#include "entity.h"

Entity* small_terrain_spawn(GFC_Vector3D position);
Entity* bridge_terrain_spawn(GFC_Vector3D position);

Entity* floor_terrain_spawn(GFC_Vector3D position);
Entity* floor_half_terrain_spawn(GFC_Vector3D position);
Entity* floor_quarter_terrain_spawn(GFC_Vector3D position);
Entity* floor_eighth_terrain_spawn(GFC_Vector3D position);
Entity* floor_sixteenth_terrain_spawn(GFC_Vector3D position);
Entity* floor_32_terrain_spawn(GFC_Vector3D position);

Entity* test_spawn(GFC_Vector3D position);

#endif