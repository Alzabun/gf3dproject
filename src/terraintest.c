#include "simple_logger.h" // always log stuff

#include "terraintest.h"

#include "gf3d_camera.h"
#include "gfc_vector.h"

//void terrain_think(Entity* self);
//void terrain_update(Entity* self);
//void terrain_free(Entity* self);
//void terrain_touch(Entity* self, Entity* other);


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
	//self->free = terrain_free;
	//self->think = terrain_think;
	self->position = position;
	//self->touch = terrain_touch;

	self->BoundingBox.x = position.x;
	self->BoundingBox.y = position.y;
	self->BoundingBox.z = position.z;
	// i think you can reference the scale values in blender for how big the bounding box should be
	// so the model and object are clearly not in sync because the collision for this model goes
	// way further to the left beyond its model and doesnt even go to the right at all???
	self->BoundingBox.w = 100;
	self->BoundingBox.d = 100;
	self->BoundingBox.h = 5; // idk whats with this one 
	
	self->flag = TERRAIN;

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

//void terrain_think(Entity* self) { // this thing not thinking about anything!!!
//}
//void terrain_update(Entity* self){
//}

//void terrain_touch(Entity* self, Entity* other) {
	//printf("Terrain touched Player at: x=%.2f, y=%.2f, z=%.2f, w=%.2f, d=%.2f, h=%.2f\n",
		//self->BoundingBox.x, self->BoundingBox.y, self->BoundingBox.z,
		//self->BoundingBox.w, self->BoundingBox.d, self->BoundingBox.h);
//}