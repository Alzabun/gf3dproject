#ifndef __RINGS_H__
#define __RINGS_H__

#include "entity.h"

// apply gravity and touch functionality if these are spawning from the player being damaged
Entity* rings_spawn(GFC_Vector3D position, int dropped);

// if the player has rings and gets hit, give a chance to collect most of them back by scattering them everywhere
void rings_dropped(Entity* self, GFC_Vector3D position, int amount); 

// i think i screwed myself over by handling the ring collision and dropped ring collision between 2 files so my brain broke and will
// now give health back from dropped rings with this. or this is actually the correct way of doing it... then that's cool
void give_ring(Entity* self);

#endif