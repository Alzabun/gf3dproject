#ifndef __OBSTACLES_H__
#define __OBSTACLES_H__

#include "entity.h"

Entity* spikes_spawn(GFC_Vector3D position);

Entity* spring_spawn(GFC_Vector3D position);

Entity* v_moving_platform_spawn(GFC_Vector3D position);

typedef struct {
	GFC_Vector3D position;
	GFC_Vector3D points[10];
}loopData;

Entity* loop_spawn(GFC_Vector3D position);

typedef struct {
	GFC_Vector3D position;
	int item; // 1-5, each number is associated with what you will get for breaking the box
}itemboxData;

Entity* itembox_spawn(GFC_Vector3D position, int type);

#endif