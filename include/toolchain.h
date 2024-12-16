#ifndef __TOOLCHAIN_H__
#define __TOOLCHAIN_H__

#include <SDL.h>            

#include "simple_json.h"
#include "simple_logger.h"
#include "entity.h"
#include "player.h"
#include "rings.h"
#include "terrain_hazards.h"
#include "obstacles.h"
#include "projectile.h"

void save_debug_file(char* name, char* type, float x, float y, float z);
void debug_delete(Entity* other);
void spawn_entity(char* name, char* type, GFC_Vector3D position);

#endif
