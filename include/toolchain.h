#ifndef __TOOLCHAIN_H__
#define __TOOLCHAIN_H__

#include <SDL.h>            

#include "simple_json.h"
#include "simple_logger.h"

void save_debug_file(char* name, char* type, float x, float y, float z/* , should use file name obtained from load game */);


#endif
