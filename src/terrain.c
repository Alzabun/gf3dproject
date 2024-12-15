#include "simple_logger.h" // always log stuff

#include "terrain.h"

#include "gf3d_camera.h"
#include "gfc_vector.h"

// change this to object.c or terrain.c
// structure similarily to enemy.c


Entity* floor_terrain_spawn(GFC_Vector3D position) {
	Entity* self;
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

	return self;
}

Entity* small_terrain_spawn(GFC_Vector3D position) {
	Entity* self;

	self = entity_new();
	if (!self) {
		return NULL;
	}
	self->model = gf3d_model_load("models/smallterrain.model");
	self->position = position;
	GFC_Box getBounds = self->model->bounds;

	self->BoundingBox.x = position.x + getBounds.x;
	self->BoundingBox.y = position.y + getBounds.y + 3; // weird offset
	self->BoundingBox.z = position.z + getBounds.z;

	self->BoundingBox.w = getBounds.w;
	self->BoundingBox.d = getBounds.d;
 	self->BoundingBox.h = getBounds.h + 8; // weird offset

	self->flag = TERRAIN;

	return self;
}
// no collision for some reason?????
Entity* bridge_terrain_spawn(GFC_Vector3D position) {
	Entity* self;

	self = entity_new();
	if (!self) {
		return NULL;
	}
	self->model = gf3d_model_load("models/terrainbridge.model");
	self->position = position;
	GFC_Box getBounds = self->model->bounds;

	self->BoundingBox.x = position.x + getBounds.x;
	self->BoundingBox.y = position.y + getBounds.y; // weird offset
	self->BoundingBox.z = position.z + getBounds.z;

	self->BoundingBox.w = getBounds.w;
	self->BoundingBox.d = getBounds.d;
	self->BoundingBox.h = getBounds.h; // weird offset

	self->flag = TERRAIN;

	return self;
}