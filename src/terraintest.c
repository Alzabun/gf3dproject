#include "simple_logger.h" // always log stuff

#include "terraintest.h"

#include "gf3d_camera.h"
#include "gfc_vector.h"

void terrain_think(Entity* self);
//void terrain_update(Entity* self);
void terrain_free(Entity* self);

typedef struct {
	Uint8 exist;
}TerrainData;

// if something isnt working that you thin should work then just reference player.c

Entity* terrain_spawn(GFC_Vector3D position) {
	Entity* self;
	TerrainData* exist;

	self = entity_new();
	if (!self) {
		return NULL;
	}
	self->model = gf3d_model_load("models/collisiontest.model");
	self->free = terrain_free;
	self->think = terrain_think;
	self->position = position;

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

void terrain_think(Entity* self) { // these are the actions the entity will do when the game loads
	int dx, dy;
	TerrainData* data;
	GFC_Vector2D dir_x = { 0,-1 }; // will only rotate in 2 dimensions
	GFC_Vector2D dir_y = { -1, 0 };

	if (!self || !self->data) {
		return;
	}
	data = self->data;

	dir_x = gfc_vector2d_rotate(dir_x, self->rotation.z);
	dir_y = gfc_vector2d_rotate(dir_y, self->rotation.z);

	self->rotation.z = 50;
}
// void terrain_update(Entity* self);

