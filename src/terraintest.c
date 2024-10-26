#include "simple_logger.h" // always log stuff

#include "terraintest.h"

#include "gf3d_camera.h"
#include "gfc_vector.h"

void terrain_think(Entity* self);
void terrain_update(Entity* self);
void terrain_free(Entity* self);
void terrain_touch(Entity* self, Entity* other);


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
	self->touch = terrain_touch;

	self->BoundingBox.x = position.x;
	self->BoundingBox.y = position.y;
	self->BoundingBox.z = position.z;

	self->BoundingBox.w = 5;
	self->BoundingBox.d = 5;
	self->BoundingBox.h = 5;

	// well this is a really lazy way to have collision for terrain
	// obviously the enviroment won't just be a straight line (box) but at this point i need anything i can do to test collision

	exist = gfc_allocate_array(sizeof(TerrainData), 1);
	if (exist) {
		self->data = exist;
	}
	return self;
}

// everything below this is unnecessary but you never know

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
	TerrainData* data;

	if (!self || !self->data) {
		return;
	}
	data = self->data;

	self->rotation.z = 500; // rotated to fit with the player angle but this is probably a really dumb way of doing it
}
void terrain_update(Entity* self) {
	gfc_vector3d_copy(self->position, self->position);
}

void terrain_touch(Entity* self, Entity* other) {
	slog("touch is activtating from terrain\n");
}