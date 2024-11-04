#ifndef __ENTITY_H__
#define __ENTITY_H__

#include "gfc_types.h"
#include "gfc_text.h"
#include "gfc_vector.h"
#include "gf3d_model.h"

#include "gfc_primitives.h"

// C does not have a string type by default, but you can use a makeshift one made in gfc/ i forgor / gfc string or something

typedef enum {
	// PLAYER
	PLAYER,
	// ENVIRONMENT
	TERRAIN,
	// OBSTACLE RELATED (GENERAL)
	DAMAGE, // (spikes) for enviromental/obstacles things that will damage the player upon contact
	// OBSTACLE RELATED (SPECIFIC)
	RINGS,
	DROPPED, // sub-category of RINGS: the rings that come out of a damaged player
	SPRING,
	PLATFORM,
	LOOP,
	ITEMBOX,
	// ENEMY RELATED
	ENEMY,
	PROJECTILE,
	// BOSS BATTLE
	BOSS,
	BOSS_PENDING,
	BOSS_START,
	// MISC
	IGNORE
}EntityFlag;

typedef struct Entity_S{

	Uint8 _inuse; // flag for memory management
	GFC_TextLine name; // name of entity
	GFC_Vector3D position; // where I am in space
	GFC_Vector3D rotation; // how i should rotate 
	GFC_Vector3D scale; // stretching
	Model* model; // my graphics (pointer means it will be handled by the model manager)
	//behavior
	//'*' BEFORE NAME FOR SYNTAX
	void (*think) (struct Entity_S *self); // called every frame for	the entity to decide things
	void (*update) (struct Entity_S* self); // called every frame for the entity to update its state
	int (*draw) (struct Entity_s *self); // for custom drawing code (this was void originally but is used fro the == -1 thing)
	void (*free) (struct Entity_s* self); // called when the entity is cleaned up to to clean up custom data
	void* data; // entity for custom data - for everything beyond the basics

	GFC_Box BoundingBox; // this better work
	GFC_Vector3D velocity;
	EntityFlag flag;
	void (*touch) (struct Entity_s* self, struct Entity_S* other);

	int bosshealth; // THIS IS TEMPORARY AND ONLY FOR THE BOSS BECUASE I AM NTO SPENDING MY TIME FIGURING OUT HOW TO SET BOSS'S DATA AT THIS TIME

}Entity; // you can change the name of this to the name of the game later (or dont)

//you should comment literally everything because you are probably not gonna remember what everything does
// also make sure every function in the entity.c is put in here

void entity_system_close();

/**
* @brief initialize the entity manager subsystem
* @param maxEnts how many entities can exist at the same time
*/
void entity_system_init(Uint32 maxEnts);

/**
* i dont remember if i forgot something here
*/

/**
* @brief allocated a blank entity for use
* @return NULL on failure (out of memory) or a pointer to the initailized entity
*/
Entity* entity_new();

//freeing stuff
void entity_free(Entity* self);	

// * drawing stuff *

//draw an entity
void entity_draw(Entity* self);
//draw all entities
void entity_draw_all();
// generic to simplify needing to copypaste
void entity_draw_generic();

// * thinking stuff *

// entity does something
void entity_think(Entity* self);
// all entities do something
void entity_think_all();

// * update stuff *

// entity updates
void entity_update(Entity* self);
// all entities update
void entity_update_all();


// * collision stuff *
int collisiontest(Entity* self, Entity* other);
void collision_check();

// * bounding box debugging *
void entity_show_box(Entity* self, GFC_Color color);
#endif __ENTITY_H__ // should this be here???

// * deleting stuff *
void sentence_to_death(Entity* self);