#ifndef __TOOLCHAIN_H__
#define __TOOLCHAIN_H__

#include <SDL.h>            

#include "simple_json.h"
#include "simple_logger.h"
#include "entity.h"
#include "player.h"

void save_debug_file(char* name, char* type, float x, float y, float z/* , should use file name obtained from load game */);
void debug_delete(Entity* other /* get the file name for this */);
void spawn_entity(char* name, char* type, GFC_Vector3D position);

#endif
