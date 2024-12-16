#include "simple_logger.h" // always log stuff

#include "terrain.h"

#include "gf3d_camera.h"
#include "gfc_vector.h"

// change this to object.c or terrain.c
// structure similarily to enemy.c

// TO DO:
// i have literally no idea why the collision is the way it is so all the floor bounds will be hard coded i guess
// IMPORTANT NOTE:
// the reason why the y position is offseted and the depth is multiplied by 2
// either the way im creating models for these or the way the game engine is wrong
// well the game engine expands the bounding box from the origin rather than expanding from the center (like in blender)
// meaning, only one side of a model has the correct collision while the other has nothing
// to solve this, multiply the depth by 2 then offset the position of the bounding box by the depth (not multipied by 2) to center it
// i think this fix can be applied universally if i divide the y and d in the gfc_box by 2 but im too scared thats going to break all the other collisons i have
// either way, this is a problem for just the width, the z/x and w/h are fine (for the most part)	

Entity* floor_terrain_spawn(GFC_Vector3D position) {
	Entity* self;
	GFC_Vector3D dir_z = { 0, 0, 1 };

	self = entity_new();
	if (!self) {
		return NULL;
	}
	self->model = gf3d_model_load("models/terrain_floor.model");
	self->position = position;

	self->BoundingBox.x = position.x;
	self->BoundingBox.y = position.y - 10000;
	self->BoundingBox.z = position.z - (150 / 2);

	// i think you can reference the scale values in blender for how big the bounding box should be

	self->BoundingBox.w = 500; // pretty irrelevant because this game is mainly 2d
	self->BoundingBox.d = 10000 * 2; // changes the length - good for ground
	self->BoundingBox.h = 150; // reference from (0, 0, 0) in the world for how high it should be
	
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

Entity* floor_half_terrain_spawn(GFC_Vector3D position) {
	Entity* self;

	self = entity_new();
	if (!self) {
		return NULL;
	}
	self->model = gf3d_model_load("models/terrain_floor_half.model");
	self->position = position;
	GFC_Box getBounds = self->model->bounds;

	self->BoundingBox.x = position.x;
	self->BoundingBox.y = position.y - (10000 / 2);
	self->BoundingBox.z = position.z - (150 / 2);

	self->BoundingBox.w = 500; 
	self->BoundingBox.d = (10000 / 2) * 2; 
	self->BoundingBox.h = 150; 

	self->flag = TERRAIN;

	return self;
}

Entity* floor_quarter_terrain_spawn(GFC_Vector3D position) {
	Entity* self;

	self = entity_new();
	if (!self) {
		return NULL;
	}
	self->model = gf3d_model_load("models/terrain_floor_quarter.model");
	self->position = position;
	GFC_Box getBounds = self->model->bounds;

	self->BoundingBox.x = position.x;
	self->BoundingBox.y = position.y - (10000 / 4);
	self->BoundingBox.z = position.z - (150 / 2);

	self->BoundingBox.w = 500;
	self->BoundingBox.d = (10000 / 4) * 2;
	self->BoundingBox.h = 150;

	self->flag = TERRAIN;

	return self;
}

Entity* floor_eighth_terrain_spawn(GFC_Vector3D position) {
	Entity* self;

	self = entity_new();
	if (!self) {
		return NULL;
	}
	self->model = gf3d_model_load("models/terrain_floor_eighth.model");
	self->position = position;
	GFC_Box getBounds = self->model->bounds;

	self->BoundingBox.x = position.x;
	self->BoundingBox.y = position.y - (10000/ 8);
	self->BoundingBox.z = position.z - (150 / 2);

	self->BoundingBox.w = 500;
	self->BoundingBox.d = (10000 / 8) * 2;
	self->BoundingBox.h = 150;

	self->flag = TERRAIN;

	return self;
}

Entity* floor_sixteenth_terrain_spawn(GFC_Vector3D position) {
	Entity* self;

	self = entity_new();
	if (!self) {
		return NULL;
	}
	self->model = gf3d_model_load("models/terrain_floor_sixteenth.model");
	self->position = position;
	GFC_Box getBounds = self->model->bounds;

	self->BoundingBox.x = position.x;
	self->BoundingBox.y = position.y - (10000/ 16);
	self->BoundingBox.z = position.z - (150 / 2);

	self->BoundingBox.w = 500;
	self->BoundingBox.d = (10000 / 16) * 2;
	self->BoundingBox.h = 150;

	self->flag = TERRAIN;

	return self;
}

Entity* floor_32_terrain_spawn(GFC_Vector3D position) {
	Entity* self;

	self = entity_new();
	if (!self) {
		return NULL;
	}
	self->model = gf3d_model_load("models/terrain_floor_32.model");
	self->position = position;
	GFC_Box getBounds = self->model->bounds;

	self->BoundingBox.x = position.x;
	self->BoundingBox.y = position.y - (10000 / 32);
	self->BoundingBox.z = position.z - (150 / 2);

	self->BoundingBox.w = 500;
	self->BoundingBox.d = (10000 / 32) * 2;
	self->BoundingBox.h = 150;

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