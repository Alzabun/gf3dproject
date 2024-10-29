#include "simple_logger.h" // always log stuff

#include "player.h"
#include "terraintest.h"

#include "gf3d_camera.h"
#include "gfc_vector.h"
#include "gfc_input.h"

#include "gf2d_font.h"

void player_think(Entity* self);
void player_update(Entity* self);
void player_free(Entity* self);
void player_touch(Entity* self, Entity* other);
void player_camera(Entity* self);
void player_damage(Entity* self);

const int MAXSPEED = 3;
const int JUMPTIME = 15;
const float MAXSPINDASHSPEED = 10;

typedef struct {
	GFC_Vector3D position; // self explanatory
	int jumpTime; // goes up by a number each frame or whatever
	float storedvelocity; // for spindash speed
	// BOOLEANS
	// in general, most of the int values are just booleans even though i never checked if there was a boolean
	// why didnt i check first? idk who cares this functions the same way
	int airborne; // 1 = yes, 2 = no
	int spindash; // 1 = yes, 2 = no
	int inball; // 1 = yes, 2 = no
	int rotdir; // 1 = left, 2 = right
}playerData;

Entity* player_spawn(GFC_Vector3D position) {
	Entity* self;
	playerData* data;

	self = entity_new();
	if (!self) {
		return NULL;
	}
	self->model = gf3d_model_load("models/dino.model");
	self->free = player_free;
	self->think = player_think;
	self->update = player_update; 
	self->position = position;
	self->touch = player_touch;
	self->velocity = gfc_vector3d(0, 0, 0);

	self->BoundingBox.x = position.x;
	self->BoundingBox.y = position.y;
	self->BoundingBox.z = position.z;

	self->BoundingBox.w = 5;
	self->BoundingBox.d = 5;
	self->BoundingBox.h = 5; 

	self->flag = PLAYER;

	data = gfc_allocate_array(sizeof(playerData), 1);
	if (data) {
		self->data = data;
	}
	return self;
}

void player_free(Entity* self) { // frees up entity
	playerData* data;
	if (!self) {
		return;
	}
	if (!self->data) {
		return;
	}
	data = (playerData*) self->data;
	
	free(data);
	self->data = NULL;
}

void player_think(Entity* self) { // these are the actions the entity will do when the game loads
	playerData* data;
	GFC_Vector3D dir_z = { 0, 0, 1 };

	if (!self || !self->data) {
		return;
	}
	data = self->data;
	
	// right is negative and left is positive because ?????????

	// MOVEMENT
	if (gfc_input_command_held("walkleft")) {
		/*if (self->rotation.z != 0) {
			printf("z rotation (going left): %f\n", self->rotation.z);
		}*/

		// SPEED CAP
		if (self->velocity.y >= MAXSPEED) {
			self->velocity.y = MAXSPEED;
		}
		else {
			self->velocity.y += 0.1;
		}
		// ROTATION CAP
		if (self->rotation.z <= -3.2) { 
			self->rotation.z = -3.2; 
		}
		else {
			self->rotation.z -= 0.2;
		}
	}
	else if (gfc_input_command_held("walkright")) {
		/*if (self->rotation.z != 0) {
			printf("z rotation (going right): %f\n", self->rotation.z);
		}*/

		// SPEED CAP
		if (self->velocity.y <= -MAXSPEED) {
			self->velocity.y = -MAXSPEED;
		}
		else {
			self->velocity.y -= 0.1;
		}
		// ROTATION CAP
		if (self->rotation.z >= 0) {
			self->rotation.z = 0;
		}
		else {
			self->rotation.z += 0.2;
		}
	}
	else {
		// FRICTION
		if (self->velocity.y > 0) {
			self->velocity.y -= 0.1;
			if (self->velocity.y < 0) {
				self->velocity.y = 0;
			}
		}
		else if (self->velocity.y < 0) {
			self->velocity.y += 0.1;
			if (self->velocity.y > 0) {
				self->velocity.y = 0;
			}
		}
		// ROTATION FALLBACK (stopping mid-turn) (also a left direction cap)
		if (self->rotation.z <= -1.5) { // LEFT
			
			if (self->rotation.z <= -3.2) {
				self->rotation.z = -3.2;
			}
			else {
				self->rotation.z -= 0.2;
			}
		}
		else if (self->rotation.z >= -1.5) { // RIGHT
			if (self->rotation.z >= 0) {
				self->rotation.z = 0;
				
			}
			else {
				self->rotation.z += 0.2;
			}
		}
	}

	// JUMPING
	// very similar to jumping in sonic games
	// hold longer to jump higher
	// binded to w for now, but i want it to also be binded to space except i didnt find the input documentation for space yet
	if (gfc_input_command_down("jump")) { 
		if (data->airborne == 0 && data->spindash == 0) {
			data->inball = 1;
			data->jumpTime = 0;
			self->velocity.z = 2;
			self->model = gf3d_model_load("models/dino_jump.model");
		}
	}

	if (gfc_input_command_held("jump") && data->jumpTime <= JUMPTIME && data->spindash == 0) {
		//printf("jumptime: %i \n", data->jumpTime);
		data->jumpTime += 1;
		self->velocity.z += 0.1;
	}

	// SPINDASH
	// functions just like sonic's spindash
	// hold down (s) to curl into a ball then spam jump to charge it until maximum allowed speed
	// you can also hold jump to charge it but thats not intended, though i shouldnt waste time trying to fix that since this still works
	if (gfc_input_command_held("spindash")) {
		data->spindash = 1;
		if (data->airborne == 0) {
			self->velocity.y = 0;
		}
		
		self->model = gf3d_model_load("models/dino_jump.model");

		if (gfc_input_command_down("jump") && data->airborne == 0) {
			if (data->rotdir == 1) {
				if (data->storedvelocity <= MAXSPINDASHSPEED) { // LEFT DIR SPINDASH
					data->storedvelocity += 0.5;
					//printf("increasing left velocity\n");
					//play sound or display velocity on a ui
				}
			}
			else if (data->rotdir == 2) {
				if (data->storedvelocity >= -MAXSPINDASHSPEED) { // RIGHT DIR SPINDASH
					data->storedvelocity -= 0.5;
					//printf("increased right velocity\n");
					//play sound or display velocity on a ui
				}
			}
			//printf("stored velocity: %.2f\n", data->storedvelocity);
		}
		// if charged at least once, keep spinning and scale with speed
		if (data->storedvelocity > 0) { 
			data->inball = 1; // you dont get to do dmg if you're just curled without speed
			self->rotation.y += (data->storedvelocity * 0.1); // left
		}
		else if (data->storedvelocity < 0){
			data->inball = 1; // you dont get to do dmg if you're just curled without speed
			self->rotation.y -= (data->storedvelocity * 0.1); // right
		}
		else {
			self->rotation.y = 0;
		}

		if (data->storedvelocity >= MAXSPINDASHSPEED && data->rotdir == 1) {
			data->storedvelocity = MAXSPINDASHSPEED;
		}
		else if (data->storedvelocity <= -MAXSPINDASHSPEED && data->rotdir == 2) {
			data->storedvelocity = -MAXSPINDASHSPEED;
		}
	}
	else {
		data->spindash = 0;
		if (data->storedvelocity != 0) {
			// implement damage while you still have speed from spindashing (and still in a ball) later
			self->velocity.y = data->storedvelocity;
			data->storedvelocity = 0;
		}
	}
}
void player_update(Entity* self) {
	playerData* data; 

	if (!self) {
		return;
	}
	data = self->data;

	data->airborne = 1; // assume airborne unless a collision happens

	// for movement speed tracking and changing
	self->position.x += self->velocity.x;
	self->position.y += self->velocity.y;
	self->position.z += self->velocity.z;

	/*if (self->velocity.y != 0) {
		printf("y velocity: %f\n", self->velocity.y);
	}*/

	// for keeping the bounding box position consistent
	// changing bounding box to be set to its velocity causes problems that idk if im supposed to fix or not
	self->BoundingBox.x = self->position.x;
	self->BoundingBox.y = self->position.y;
	self->BoundingBox.z = self->position.z;

	//printf("rotdir: %i\n", data->rotdir);

	// GRAVITY
	// DONT MOVE THIS, IT DOESNT WORK OTHERWISE
	if (data->airborne == 1) {
		self->velocity.z -= 0.1;

		if (data->spindash == 0) { // prevent animation when spindashing unless its from the spindash itself
			self->rotation.y += 0.1;
		}
		
		// respawn player on top of the map if they fall into the void
		if (self->position.z <= -250) {
			self->position.z = 200;
			self->position.x = 0;
			self->position.y = 0;
			self->velocity.z = 0;
		}
		// this is obviously going to cause problems when t	here has to be collisions with walls and probably enemies
		// im pretty sure i can use the bounding box sides to fix this tho, but that means changing a lot of how this works right now
	}

	if (self->rotation.z >= -1.5) { // idk if i had to put an extra rotation detection here (probably not)
		data->rotdir = 2; // facing right
	}
	else {
		data->rotdir = 1; // facing left
	}

	player_camera(self); // this is fine for now
	/*
	printf("player box: x=%.2f, y=%.2f, z=%.2f, w=%.2f, d=%.2f, h=%.2f\n",
       self->BoundingBox.x, self->BoundingBox.y, self->BoundingBox.z,
       self->BoundingBox.w, self->BoundingBox.d, self->BoundingBox.h);
	printf("player position: x=%.2f, y=%.2f, z=%.2f\n",
		self->position.x, self->position.y, self->position.z);
	*/
}

void player_camera(Entity* self) {
	GFC_Vector3D lookTarget, camera, dir = { 0 };

	// CAMERA SYSTEM
	// if necessary for the final project or not, i originally want this to be like a modern sonic boost formula stage
	// what that means: gameplay switches from 3d to 2d, determined by the camera angle
	// that would require recoding a lot of the controls though because theyre based off 2d only
	// it would be cool to make this, but if i dont have time to or if there's no point then whatever

	gfc_vector3d_copy(lookTarget, self->position);

	lookTarget.z += 5; // this changes the offset of the camera
	dir.x = 500.0; // was 50
	//gf3d_camera_look_at(lookTarget, const GFC_Vector3D *position);
	// could change to 3d pov just by changing the values of this and the camera below
	//gfc_vector3d_rotate_about_z(&dir, self->rotation.z); // the rotation the camera will go along with
	gfc_vector3d_sub(camera, self->position, dir);
	camera.z += 15; // changes angle of camera from by rotating around the player's z 
	gf3d_camera_look_at(lookTarget, &camera);

	//data->cameraPitch += dy * 0.01;
}
	
void player_touch(Entity* self, Entity* other) {
	playerData* data;
	data = self->data;

	if (other->flag == TERRAIN) { // only happening once then never again for some reason
		//slog("collided with terrain");
		self->velocity.z = 0;
		if (data->airborne == 1 && data->inball == 1) {
			data->inball = 0;
		}
		data->airborne = 0;
		if (data->spindash == 0) {
			self->model = gf3d_model_load("models/dino.model");
			self->rotation.y = 0;
		}
	}
	else if (other->flag == ENEMY) {
		if (data->inball) {
			// kill the enemy
			self->velocity.z = 2; // reject gravity 
			other->model = gf3d_model_load("models/explosion.model"); // refer to note
			// NOTE: entity freeing happens too fast for this to show, so fix later if theres time since it's not that important
			sentence_to_death(other);
			slog("collided with enemy (attack)");
		}
		else {
			// do damage
			slog("collided with enemy (damage)");
		}
	}
	else if (other->flag == DAMAGE) {
		slog("collided with something dangerous");
	}
	else if (other->flag == IGNORE) {
		slog("collided with something unimportant");
	}
	else {
		//slog("collided with misc?");
		self->velocity.z = 0;
		data->airborne = 0;
		if (data->spindash == 0) {
			self->model = gf3d_model_load("models/dino.model");
			self->rotation.y = 0;
		}
	}
}

void player_damage(Entity* self) {

}