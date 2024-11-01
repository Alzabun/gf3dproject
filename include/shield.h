#ifndef __SHIELD_H__
#define __SHIELD_H__

#include "entity.h"

Entity* shield_spawn(GFC_Vector3D position, Entity* player, int type);
void shield_free(Entity* self);

#endif