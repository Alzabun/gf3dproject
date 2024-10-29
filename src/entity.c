#include "simple_logger.h"
#include "gfc_matrix.h"
#include "entity.h"

#include "gfc_primitives.h"
#include "gf3d_obj_load.h"
#include "gfc_color.h"
#include "gf3d_draw.h"

//entity stuff should be building on top of gf3d_mesh.c stuff
// make sure to constantly compile to check for errors
// also, some things may be missing or if statements may be ordered incorrectly
// this is how to make a resource manager, and this is copied but may or may not be modified based on whatever the game needs
// if its missing, you should be able to fix it or add on to it... probably

// for another example of what render systems will look like, you can look at mesh, sound, and something else i forgot

// i had no time to copy it but from game.c, you can make a dino.h and dino.c as a file for a specific entity to do stuff

// fix entity.c 
// the game is black becuae of entity.c
typedef struct {

	Entity* entityList;
	Uint32 entityMax;

}EntityManager;

// we will have only ONE entity manager, so this will be a global function
// static means its basically private to this file
static EntityManager entity_manager = {0}; 

void entity_system_close() { // cleans the entities when it closes
	int i;
	for (i = 0; i < entity_manager.entityMax; i++) {
		if (!entity_manager.entityList[i]._inuse) {
			continue; // skip ones not set
		}
		entity_free(&entity_manager.entityList[i]);
	}
	free(entity_manager.entityList);
	memset(&entity_manager, 0, sizeof(EntityManager));
}

void entity_system_init(Uint32 maxEnts) {
	if (entity_manager.entityList) {
		slog("entity manager exists"); // checks if this actually works first
		return;
	}
	if (!maxEnts) {
		slog("cant allocate 0 entities in the entity manager"); // dont allocate nothing
		return;
	}
	entity_manager.entityList = gfc_allocate_array(sizeof(Entity), maxEnts);
	if (!entity_manager.entityList) {
		slog("failed to allocate %i entities for the entity manager", maxEnts); // too much entities
		return;
	}
	entity_manager.entityMax = maxEnts;
	atexit(entity_system_close);
}

// * drawing stuff *

void entity_draw_generic(Entity* self) { // this is where we make a matrix of 4x4 to set the layout of entities to spawn
	GFC_Matrix4 matrix;

	if (!self) {
		return;
	}

	/*if (self->draw) {
		if (self->draw(self) == -1) { // what
			return;
		}
	}*/

	gfc_matrix4_from_vectors(
		matrix,
		self->position,
		self->rotation,
		self->scale);

	gf3d_model_draw(
		self->model,
		matrix, // no idea whats supposed to actually be here
		GFC_COLOR_WHITE,
		NULL, // becuase we modified this in gf3d_model.h
		// every place that has gf3d_model_draw, just replcae the Light parameter with null for now
		// and gf3d_model_draw_all_meshes
		// the error logs should tell you where it needs the new parameter nayway
		0);
	// you can check the defintion of this function, these parameters are only based off of it

}

// a copied version of the function abovee but instead generic can be used to simplify the self-> process after calling draw

// theres supposed to be things like this for draw_all and think_all and think and draw

void entity_draw(Entity *self) {
	GFC_Color red = { 1, 0, 0, 1 };

	if (!self) {
		return;
	}

	if (self->draw) {
		if (self->draw(self) == -1) {
			return;
		}
	}

	entity_draw_generic(self);
	entity_show_box(self, red); // for debugging reasons, shows bounding box for every entity
}

void entity_draw_all() {
	int i;
	for (i = 0; i < entity_manager.entityMax; i++) {
		if (!entity_manager.entityList[i]._inuse) {
			continue; // skip ones not set
		}
		entity_draw(&entity_manager.entityList[i]);
	}
}

void entity_think(Entity* self) {
	if (!self) {
		return;
	}
	if (self->think) {
		self->think(self);
	}
}

void entity_think_all() {
	int i;
	for (i = 0; i < entity_manager.entityMax; i++) {
		if (!entity_manager.entityList[i]._inuse) { // no idea if this is how to do it
			continue;
		}
		entity_think(&entity_manager.entityList[i]); // neither for this one
	}
}

Entity* entity_new() {
	int i;
	for (i = 0; i < entity_manager.entityMax; i++) {
		if (entity_manager.entityList[i]._inuse) {
			continue; // skips the ones already spawned in
		}
		memset(&entity_manager.entityList[i], 0, sizeof(Entity)); //clear it out just incase theres memory being used still for some reason
		entity_manager.entityList[i]._inuse = 1;
		entity_manager.entityList[i].scale = gfc_vector3d(1, 1, 1);
		// any default values should be set
		return &entity_manager.entityList[i];
	}
	return NULL; //there are no more entities
}

void entity_free(Entity* self) {
	if (!self) {
		return; // check if its null first because you cant trust pointers
	}
	// these clear up memory just in case its being used already for some reason
	if (self->free) {
		self->free(self); // this is how you call a function pointer
	}
	gf3d_model_free(self->model);
	memset(self, 0, sizeof(Entity));


}

// * updating stuff * 

void entity_update(Entity* self) {

	if (!self) {
		return;
	}

	if (self->update) {
		self->update(self);
	}

	//updaet stuff go here
}

void entity_update_all() {
	int i;
	for (i = 0; i < entity_manager.entityMax; i++) {
		if (!entity_manager.entityList[i]._inuse) {
			continue;
		}
		entity_update(&entity_manager.entityList[i]);
	}
	collision_check();
}

//collision test

int collisiontest(Entity* self, Entity* other) {
	GFC_Box* firstbox = &self->BoundingBox;
	GFC_Box* secondbox = &other->BoundingBox;
	// should check if its actually inside a box

	if (firstbox->x + firstbox->w <= secondbox->x || secondbox->x + secondbox->w <= firstbox->x) {
		return 0;
	}

	if (firstbox->y + firstbox->d <= secondbox->y || secondbox->y + secondbox->d <= firstbox->y) {
		return 0;
	}

	if (firstbox->z + firstbox->h <= secondbox->z || secondbox->z + secondbox->h <= firstbox->z) {
		return 0;
	}

	return 1;
}

void collision_check() {
	for (int i = 0; i < entity_manager.entityMax; i++) {
		Entity* first = &entity_manager.entityList[i];
		if (!first->_inuse) {
			continue;
		}

		for (int j = i + 1; j < entity_manager.entityMax; j++) {
			Entity* second = &entity_manager.entityList[j];
			if (!second->_inuse) { 
				continue;
			}

			if (collisiontest(first, second) == 1) { // when the bounding boxes intersect for the two entities, they will have both their touch functions activated
				if (first->touch) {
					first->touch(first, second);
				}
				if (second->touch) {
					second->touch(second, first);
				}
			}
		}
	}
}

void entity_touch(Entity* self) {
	// idk if i needed this actually
}

void entity_touch_all(Entity* self) {
	for (int i = 0; i < entity_manager.entityMax; i++) {
		if (!entity_manager.entityList[i]._inuse) {
			continue;
		}
		entity_touch(&entity_manager.entityList[i]);
	}
}

void entity_show_box(Entity* self, GFC_Color color) {
	GFC_Box box;
	GFC_Vector3D position, rotation, scale;
	// r g b a (only from 1 to 0 for each)
	// so from what im understanding, this is either not working how it should be or im misunderstanding something
	// more specifically, this box is not reflecting what the box actually looks like
	// this is giving me a headache i think i should just start making other deliverables while the collision is in this state


	if (!self) {
		return;
	}

	position = self->position;
	rotation = self->rotation;
	scale = self->scale;

	box.x = self->BoundingBox.x;
	box.y = self->BoundingBox.y;
	box.z = self->BoundingBox.z;

	box.w = self->BoundingBox.w;
	box.d = self->BoundingBox.d;
	box.h = self->BoundingBox.h;

	self->flag = IGNORE;

	//printf("box position: x=%.2f, y=%.2f, z=%.2f\n", self->position.x, self->position.y, self->position.z);
	gf3d_draw_cube_solid(box, position, rotation, scale, color);
}