#ifndef __OBSTACLES_H__
#define __OBSTACLES_H__

#include "entity.h"
#include "projectile.h"
#include "entitykiller.h"

Entity* spikes_spawn(GFC_Vector3D position);

Entity* spring_yellow_spawn(GFC_Vector3D position);

Entity* v_moving_platform_spawn(GFC_Vector3D position);

typedef struct {
	GFC_Vector3D position;
	GFC_Vector3D points_forward[10];
	GFC_Vector3D points_backward[10];
}loopData;

Entity* loop_spawn(GFC_Vector3D position);

typedef struct {
	GFC_Vector3D position;
	int item; // 1-5, each number is associated with what you will get for breaking the box
}itemboxData;

Entity* itembox_spawn(GFC_Vector3D position, int type);

Entity* goal_spawn(GFC_Vector3D position);

Entity* collapsing_bridge_spawn(GFC_Vector3D position);

Entity* bubble_spawner_spawn(GFC_Vector3D position);

Entity* collapsing_bridge_spawn(GFC_Vector3D position);

Entity* platform_sinking_spawn(GFC_Vector3D position);

Entity* platform_crushing_spawn(GFC_Vector3D position);

Entity* laser_pattern_spawn(GFC_Vector3D position, float delay);

#endif