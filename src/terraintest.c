#include "simple_logger.h" // always log stuff

#include "terraintest.h"

#include "gf3d_camera.h"
#include "gfc_vector.h"

// change this to object.c or terrain.c
// structure similarily to enemy.c

typedef struct {
	Uint8 exist;
}TerrainData;

Entity* terrain_spawn(GFC_Vector3D position) {
	Entity* self;
	TerrainData* exist;
	GFC_Vector3D dir_z = { 0, 0, 1 };

	self = entity_new();
	if (!self) {
		return NULL;
	}
	self->model = gf3d_model_load("models/collisiontest.model");
	self->position = position;

	self->BoundingBox.x = position.x;
	self->BoundingBox.y = position.y;
	self->BoundingBox.z = position.z;

	// i think you can reference the scale values in blender for how big the bounding box should be

	// i am dumb, idk why i kept confusing how this worked
	// the bounding box scales FROM the position set here IT DOESNT SCALE FROM THE MIDDLE OF TWO IMAGINARY POINTS OR SOMETHING
	// THIS IS WHY THE BOX ONLY SEEMS TO STRETCH LEFT, IT'S ACTUALLY WORKING IT JUST DOESNT WORK THE WAY I THOUGHT IT DOES
	// IDK WHY IT TOOK ME THIS LONG TO REALIZE BUT WHATEVER
	self->BoundingBox.w = 500; // pretty irrelevant because this game is mainly 2d
	self->BoundingBox.d = 10000; // changes the length - good for ground
	self->BoundingBox.h = 45; // reference from (0, 0, 0) in the world for how high it should be
	
	self->flag = TERRAIN;

	// spawnin every terrain and world obstacle as an entity seems dumb because ill have to manually spawn each terrain and align them accordingly
	// but like.. this is how a level editor works, i'm just doing the labour

	exist = gfc_allocate_array(sizeof(TerrainData), 1);
	if (exist) {
		self->data = exist;
	}
	return self;
}

void terrain_free(Entity* self) { // frees up entity
	TerrainData* exist;
	if (!self) {
		return;
	}
	if (!self->data) {
		return;
	}
	exist = (TerrainData*) self->data;
	
	free(exist);
	self->data = NULL;
}

Entity* test_spawn(GFC_Vector3D position) {
	Entity* self;
	TerrainData* exist;

	self = entity_new();
	if (!self) {
		return NULL;
	}
	//self->model = gf3d_model_load("models/forest.model");
	self->position = position;

	exist = gfc_allocate_array(sizeof(TerrainData), 1);
	if (exist) {
		self->data = exist;
	}
	return self;
}