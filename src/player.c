#include "simple_logger.h" // always log stuff

#include "gf3d_camera.h"
#include "gfc_vector.h"
#include "gfc_input.h"
#include "gf2d_font.h"
#include "gfc_audio.h"

#include "player.h"
#include "UI.h"
#include "boss.h"

// FINAL PROGRESSION [CURRENT]
// music/sounds [0.5/1]
// add start screen
// add minigame anyway (maybe?)
// changed pipeline
// research component (advanced collision)	
// basically just make the game look better
// TBA...
// 
// 
// maybe optional (unknown what the actual requirements are yet):
// 3d camera/controls switch toggle
// menu screen / start screen
// 
// ---------------------------------
// 
// MIDTERM PROGRESSION [ENDED]
// Deliverables Status:
// world obstacles (5/5):
// springs [x]
// moving platforms [x] 
// spikes [x]
// loops [x]
// item box (can also contain power-ups) [x]
// 
// power ups (5/5):
// fire shield [x]
// electricity shield [x]
// bubble shield [x] 
// normal shield [x]
// 
// velocity increase power-up
// OR
// magnet shield which collects nearby rings [i would like to add this instead but it's fine for now]
// OR
// invincibility power-up [x]
//
// enemies (5/5):
// generic enemy [x]
// generic projectile enemy [x]
// flying projectile/generic enemy [x]
// bomb projectile enemy [x]
// spiked enemy [x]
//
// boss battle (1/1)
// Yes!
// 
// mini-game (0/1)
// yeah this isnt happening as much as i want to make it
// if i were to make it i predict it'd take 1-2 days (or 1 day of spending the whole day on this like how i have been but the whole week instead)

// Common Deliverables (5/5):
// UI changes [x]
// entity system [x]
// basic controls [x]
// basic collision [x]
// contained game world [x] [background no work]

// Optional Requirements (for my own satisfaction):
// rings (health) [x] 
// ring re-collection [half x]
// music/sounds [0.5/1] -> this was actually a final deliverable apparently
// 3d camera/controls switch toggle
// menu screen / start screen
// more...


const float MAXSPEED = 5;
const float JUMPTIME = 10;
const float MAXSPINDASHSPEED = 10;
float RECOIL = 2; // when you bounce from doing/taking damage
const float IFRAMES = 10; // roughly 3 seconds? [USE DELTA TIME INSTEAD BUT FIX LATER]
const float OXYGEN = 30; // 30 seconds? from using the deltatime i have here
const float DELTATIME = 0.025; // ok

// ***** MOVED STRUCT TO PLAYER.H ******

// SOUND EFFECTS
// TO DO: actually figure out how this works, my brain is fried so i cant right now
//GFC_Sound* jump = gfc_sound_load("sounds/jump", 1 ,0); // not how it works
//Mix_FreeChunk(jump); // use this a lot

Entity* player_spawn(GFC_Vector3D position) {
	Entity* self;
	playerData* data;
	menuState currentState = get_menu();

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

	// MUSIC
	// MP3S DONT WORK FOR SOME REASON SO USE WAV INSTEAD (unless its because of the mp3 convertor website im using)

	data = gfc_allocate_array(sizeof(playerData), 1);
	if (data) {

		data->health = 0; // no rings by default
		data->invincibility = 0; // no i-frames by default
		data->deltatime = 0; // upon game begin
		data->lives = 3; //typical

		data->normal_music = gfc_sound_load_music("music/windyvalley.wav");
		Mix_PlayMusic(data->normal_music, -1);

		data->jump = gfc_sound_load("sounds/jump.wav", 1, 0);

		data->boss_music = gfc_sound_load_music("music/bigarms.wav");
		data->wintheme = gfc_sound_load_music("music/win.wav");

		data->killedboss = 0;

		data->oxygen = OXYGEN;

		if (currentState == DEBUG) {
			data->debugmode = 1;
			data->indebug = 1;
		}

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

	if (gfc_input_key_pressed("g") && data->debugmode == 1) {
		data->indebug ^= 1;
		data->entitycycle = 0;
		self->velocity.x = 0;
		self->velocity.y = 0;
		self->velocity.z = 0;
		self->rotation.x = 0;
		self->rotation.y = 0;
		self->rotation.z = 0;
		
	}

	if (data->indebug == 1) {
		debug_think(self); // go to bottom of this file
		return;
	}
	
	// right is negative and left is positive because ?????????

	// MOVEMENT
	// TO DO:
	// this whole system will probably always be slightly adjusted
	// my main problem with it is that it feels way too slippery
	// not really important to change for now since there isn't any real platforming to do, but fix it when you can
	if (gfc_input_command_held("walkleft") && data->spindash == 0) {

		// SPEED CAP
		if (self->velocity.y >= MAXSPEED + data->storedvelocity) {
			self->velocity.y = MAXSPEED + data->storedvelocity;
		}
		else {
			self->velocity.y += 0.075;
		}
		// ROTATION CAP
		if (self->rotation.z <= -3.2) { 
			self->rotation.z = -3.2; 
		}
		else {
			self->rotation.z -= 0.4;
		}
	}
	else if (gfc_input_command_held("walkright") && data->spindash == 0) {

		// SPEED CAP
		if (self->velocity.y <= -MAXSPEED - data->storedvelocity) {
			self->velocity.y = -MAXSPEED - data->storedvelocity;
		}
		else {
			self->velocity.y -= 0.075;
		}
		// ROTATION CAP
		if (self->rotation.z >= 0) {
			self->rotation.z = 0;
		}
		else {
			self->rotation.z += 0.4;
		}
	}
	else {
		// FRICTION
		// AFFECTED BY ICE (even though the way i have it now makes it feel like youre already on ice FIX THAT LPEASE)
		if (self->velocity.y > 0) {
			if (data->inIce) {
				self->velocity.y -= 0.05;
			}
			else {
				self->velocity.y -= 0.15;
			}

			if (data->spindash == 1) {
				self->rotation.y += (self->velocity.y * 0.1);
			}
			if (self->velocity.y < 0) {
				self->velocity.y = 0;
			}
		}
		else if (self->velocity.y < 0) {
			if (data->inIce) {
				self->velocity.y += 0.05;
			}
			else {
				self->velocity.y += 0.15;
			}

			if (data->spindash == 1) {
				self->rotation.y -= (self->velocity.y * 0.1);
			}
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
			gfc_sound_play(data->jump, 0, 1, 0, -1);
			data->inball = 1;
			data->jumpTime = 0;
			if (data->onPlatform == 1) { // allow jumping off platforms (temporary implementation because this gives you an unintentional jump boost)
				self->position.z += 8; // 8 is a big enough number to disconnect from the platform apparently
				data->onPlatform = 0;
			}
			self->velocity.z = 2;
			
			if (data->invincibility <= 0) {
				self->model = gf3d_model_load("models/dino_jump.model");
			}
			else {
				self->model = gf3d_model_load("models/dino_jump_iframe.model");
			}
		}
	}

	if (gfc_input_command_held("jump") && data->jumpTime <= JUMPTIME && data->spindash == 0) {
		if (data->inSand) {
			data->jumpTime += 2;
			self->velocity.z += 0.05;
		}
		else {
			data->jumpTime += 1;
			self->velocity.z += 0.1;
		}
		
	}

	// SPINDASH
	// functions just like sonic's spindash
	// hold down (s) to curl into a ball then spam jump to charge it until maximum allowed speed
	// you can also hold jump to charge it but thats not intended, though i shouldnt waste time trying to fix that since this still works
	if (gfc_input_command_held("spindash")) {
		data->spindash = 1;
		data->inball = 1;
		
		if (data->invincibility <= 0) {
			self->model = gf3d_model_load("models/dino_jump.model");
		}
		else {
			self->model = gf3d_model_load("models/dino_jump_iframe.model");
		}

		if (gfc_input_command_down("jump") && data->airborne == 0) {
			self->velocity.y = 0;
			if (data->rotdir == 1) {
				if (data->storedvelocity <= MAXSPINDASHSPEED) { // LEFT DIR SPINDASH
					data->storedvelocity += 0.5;
					//play sound or display velocity on a ui
				}
			}
			else if (data->rotdir == 2) {
				if (data->storedvelocity >= -MAXSPINDASHSPEED) { // RIGHT DIR SPINDASH
					data->storedvelocity -= 0.5;
					//play sound or display velocity on a ui
				}
			}
		}
		// if charged at least once, keep spinning and scale with speed
		if (data->storedvelocity > 0) { 
			data->inball = 1; // you dont get to do dmg if you're just curled without speed
			self->rotation.y += (data->storedvelocity * 0.1); // left
		}
		else if (data->storedvelocity < 0){
			if (data->shield) { // prevent multiple visual shields at once
				sentence_to_death(data->shield);
			} // you dont get to do dmg if you're just curled without speed
			self->rotation.y -= (data->storedvelocity * 0.1); // right
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
	
	//slog("stored velocity: %.2f", data->storedvelocity);
	//slog("in ball: %i", data->inball);

	// POWER-UP MODIFICATIONS

	if (gfc_input_command_down("powerupability")) {
		// FIRE SHIELD
		if (data->fireshield == 1) {
			if (data->airborne == 1) {
				data->inball = 1;
				if (data->rotdir == 1 && self->velocity.y <= MAXSPEED) {
					self->velocity.y += MAXSPEED * 0.75;
				}
				else if (data->rotdir == 2 && self->velocity.y >= -MAXSPEED) {
					self->velocity.y += MAXSPEED * -0.75;
				}
			}
		}
		// BUBBLE SHIELD
		else if (data->bubbleshield == 1) {
			if (data->airborne == 1) {
				self->velocity.z = -4;
				data->bubblebounce = 1;
			}
		}
		// ELECTRICITY SHIELD
		// TO DO: flash the screen white upon use
		// attract rings close to it. not necessary to implement, but it would be cool i think
		else if (data->electricityshield == 1 && data->doublejumped == 0) {
			if (data->airborne == 1) {
				self->velocity.z = 3;
				data->doublejumped = 1;
			}
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

	//slog("x, y, z positions: %.2f, %.2f, %2.f", self->position.x, self->position.y, self->position.z);

	// for movement speed tracking and changing
	self->position.x += self->velocity.x;
	self->position.y += self->velocity.y;
	self->position.z += self->velocity.z;

	// for keeping the bounding box position consistent
	// changing bounding box to be set to its velocity causes problems that idk if im supposed to fix or not (me from the future: no not really)
	self->BoundingBox.x = self->position.x;
	self->BoundingBox.y = self->position.y;
	self->BoundingBox.z = self->position.z;

	//printf("rotdir: %i\n", data->rotdir);

	if (data->lives < 0) {
		sentence_to_death(self); // GAME OVER
	}

	// IFRAME TIMER
	if (data->invincibility > 0) {
		data->invincibility -= 0.1;
		// TO DO:
		// every model change has a check to see if theres invinciblity to make the player white to indicate iframes are on
		// this is kind of a boring way of doing it but id have to figure out a blinking or hurt animation later
		// also this seems to be buggy because the model dosnt change if its in the middle of jumping or something, which will mislead people
		// probably make a constant check to see state of model instead of per input
	}

	// GRAVITY
	if (data->airborne == 1 && data->indebug == 0) {
		self->model = gf3d_model_load("models/dino_jump.model");
		//account for enviromment conditions
		if (data->inWater) {
			self->velocity.z -= 0.05; // lower gravity for a slowness illusion
		}
		else if (data->inSand) {
			self->velocity.z = 0; // jump out of the sand
		}
		else {
			self->velocity.z -= 0.1; // default gravity
		}

		if (data->spindash == 0) { // prevent animation when spindashing unless its from the spindash itself
			self->rotation.y += 0.1;
		}

		// respawn player on top of the map if they fall into the void
		if (self->position.z <= -500) { // was -250, this thing should be changed because things are changing
			self->position.z = 0;
			self->position.x = 0;
			self->position.y = 500;
			self->velocity.z = 0;
		}
	}

	// OXYGEN (WATER)
	if (data->inWater && data->indebug == 0) {
		data->oxygen -= DELTATIME;
		if (data->oxygen <= 10) {
			// insert drowning music here (make sure it only plays once)
		}
		if (data->oxygen <= 0) {
			player_die(self);
		}
	}

	// ROTATION DIRECTION DETECTION
	if (self->rotation.z >= -1.5) { // idk if i had to put an extra rotation detection here (probably not)
		data->rotdir = 2; // facing right
	}
	else {
		data->rotdir = 1; // facing left
	}

	// SHIELD REMOVAL
	if (data->shield && data->haspowerup == 0) { // prevent multiple visual shields at once
		sentence_to_death(data->shield);
	}

	// LOOP LIST
	if (data->inloop == 1 && data->indebug == 0) {
		player_loop(self, data->thisloop);
	}

	// X-POSITION FALLBACK (placeholder until 3rd dimension is utilized more)
	if (data->debugmode == 0) {
		if (self->position.x > 0) {
			self->position.x -= 0.1;
			if (self->position.x < 0) {
				self->position.x = 0;
			}
		}
		else if (self->position.x < 0) {
			self->position.x += 0.1;
			if (self->position.x > 0) {
				self->position.x = 0;
			}
		}
	}
	

	// CAMERA
	player_camera(self); // this is fine for now

	// UI UPDATES
	data->speed_y = fabs(self->velocity.y);
	data->speed_z = fabs(self->velocity.z);
	data->deltatime += DELTATIME; // this is NOT how time works but WHATEVER it's CLOSE ENOUGH	
	//data->speed_x = self->velocity.x;
	if (data->debugmode == 1) {
		data->position_x = self->position.x;
		data->position_y = self->position.y;
		data->position_z = self->position.z;
	}
	prepare_UI(data);

	// UPDATES FOR BOSS DATA
	//getPlayer(data);
}

void player_camera(Entity* self) {
	playerData* data;
	GFC_Vector3D lookTarget, camera, dir = { 0 };

	if (!self) {
		return;
	}
	data = self->data;
	

	// CAMERA SYSTEM
	// if necessary for the final project or not, i originally want this to be like a modern sonic boost formula stage
	// what that means: gameplay switches from 3d to 2d, determined by the camera angle
	// that would require recoding a lot of the controls though because theyre based off 2d only
	// it would be cool to make this, but if i dont have time to or if there's no point then whatever

	gfc_vector3d_copy(lookTarget, self->position);

	//lookTarget.z += 0; // this changes the offset of the camera

	dir.x = 175.0; // set to 500 for bounding box view, 175 (may be adjusted) for normal gameplay

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

	if (!self) {
		return;
	}
	data = self->data;

	if (data->indebug == 1) {
		return; // no collisions while you are a magical flying entity
	}

	//BUBBLE POWERUP CHANGES
	if (data->bubblebounce == 1) {
		if (other->flag == TERRAIN || other->flag == PLATFORM || other->flag == ITEMBOX) {
			data->bubblebounce = 0;
			self->velocity.z = 4;
			self->position.z += 5; // prevents player from falling through floor and from randomly not bouncing (lazy way to fix it? i dont really think so)
			if (self->position.y <= other->position.y) {
				self->position.y = other->position.y + 5;
			}
			return;
		}
		if (other->flag == ENEMY) {
			other->model = gf3d_model_load("models/explosion.model"); // NOTE: entity freeing happens too fast for this to show, so fix later if theres time since it's not that important
			sentence_to_death(other);
			return;
		}
	}
	// ELECTRICITY POWERUP CHANGES
	if (data->electricityshield == 1) {
		if (other->flag == TERRAIN || other->flag == SPRING || other->flag == PLATFORM || other->flag == ITEMBOX) {
			data->doublejumped = 0;
		}
	}

	// TERRAIN-SPECIFIC COLLISION

	if (other->flag == SAND) {
		data->inSand = 1;
		// slowing + sinking
		// jump should be "stuck" too
		self->velocity.z -= 0.05;
		self->velocity.y *= 0.5;
		// make player get hurt at the bottom of the sand's hitbox (floor of bounding box)
	}
	else {
		data->inSand = 0;
	}

	if (other->flag == WATER) {
		data->inWater = 1;
	}
	else {
		data->inWater = 0;
		data->oxygen = OXYGEN;
	}

	if (other->flag == OIL) {
		data->inOil = 1;
	}
	else {
		data->inOil = 0;
	}

	if (other->flag == ICE) {
		data->inIce = 1;
	}
	else {
		data->inIce = 0;
	}

	// TOUCH COLLISIONS

	if (other->flag == TERRAIN) { 
		//slog("collided with terrain");
		self->velocity.z = 0;
		data->airborne = 0;
		data->onPlatform = 0;
		data->inball = 0; // fix later since this is probably gonna conflict with spindashing
		
		if (data->spindash == 0) {
			self->rotation.y = 0;
			if (data->invincibility <= 0) {
				self->model = gf3d_model_load("models/dino.model");
			}
			else {
				self->model = gf3d_model_load("models/dino_iframe.model");
			}
		}
	}

	if (other->flag == ENEMY) { // kill the enemy or take damage from the enemy
		if (data->inball == 1 || data->storedvelocity > 0) { // i might have only needed to check for stored velocity for this to work
			if (data->spindash != 1) { // not implemented correctly, fix later (spindash is not being detected when the code gets here)
				self->velocity.z = RECOIL; // reject gravity 
			}
			other->model = gf3d_model_load("models/explosion.model"); // NOTE: entity freeing happens too fast for this to show, so fix later if theres time since it's not that important
			sentence_to_death(other);
			//slog("collided with enemy (attack)");
		}
		else { // take damage
			player_damage(self);
			//slog("collided with enemy (damage)");
		}
	}

	if (other->flag == PROJECTILE || other->flag == DAMAGE) {
		player_damage(self); // take damage from projectile
		// slog("collided with something dangerous");
	}

	if (other->flag == RINGS) {
		data->health += 1;
		//play sound
		sentence_to_death(other);
	}

	if (other->flag == SPRING) {
		self->velocity.z = 5;
		//slog("collided with spring");
		// NOTE: this is assuming it's a grounded spring. orientation will chanage velocity direction but that's not added yet
	}

	if (other->flag == PLATFORM) {
		self->position.z = other->position.z;
		self->velocity.z = 0;
		data->onPlatform = 1;
		data->airborne = 0;
		if (data->spindash == 0) {
			self->rotation.y = 0;
			if (data->invincibility <= 0) {
				self->model = gf3d_model_load("models/dino.model");
			}
			else {
				self->model = gf3d_model_load("models/dino_iframe.model");
			}
		}
	}

	if (other->flag == LOOP) {
		if (data->inloop == 0) {
			data->inloop = 1;
			data->thisloop = (loopData*)other->data; // this is how to get control over the loopdata struct here
			data->currentpoint = 0;
		}
	}

	if (other->flag == ITEMBOX) {
		//slog("touched itembox");
		if (data->inball == 1 || data->storedvelocity > 0) { // why isnt this working????
			if (data->spindash != 1) {
				self->velocity.z = RECOIL;
			}

			data->thispowerup = (itemboxData*)other->data;
			player_powerup(self, data->thispowerup);

			other->model = gf3d_model_load("models/explosion.model"); // NOTE: entity freeing happens too fast for this to show, so fix later if theres time since it's not that important
			sentence_to_death(other);
		}
	}

	if (other->flag == IGNORE || other->flag == DROPPED) {
		//slog("ignored a collision");
		// collide with rings unless they're the dropped ones from taking damage
		// to prevent instantly picking them back up
		// rings.c handles the DROPPED flag collision
	}

	if (other->flag == BUBBLE) {
		// insert bubble sound effect here
		// stop drowning music here unless i can figure out how to make it do that by itself in the drowning part of the code (not done here)
		data->oxygen = OXYGEN;
		sentence_to_death(other);
	}

	// BOSS LOGIC
	if (other->flag == BOSS_PENDING) {
		// prepare thy boss
		other->flag = BOSS_START;
		other->BoundingBox.w = 10;
		other->BoundingBox.d = 20;
		other->BoundingBox.h = 50;

		Mix_HaltMusic();
		Mix_PlayMusic(data->boss_music, -1);

		//slog("activated bounding box change");
	}

	if (other->flag == BOSS){
		if (data->inball == 1 || data->storedvelocity > 0) { // i might have only needed to check for stored velocity for this to work
			if (data->spindash != 1) { // not implemented correctly, fix later (spindash is not being detected when the code gets here)
				if (data->rotdir == 1) {
					self->velocity.y = -2;
					self->velocity.z = 1;
				}
				else if (data->rotdir == 2) {
					self->velocity.y = -2;
					self->velocity.z = 1;
				}
			}
			if (other->bosshealth > 0) {
				other->bosshealth -= 1;
			}
			else {
				sentence_to_death(other);
				Mix_HaltMusic();
				data->killedboss = 1;
				Mix_PlayMusic(data->wintheme, 0);
			}
			
		}
		else { // take damage
			player_damage(self);
		}
	}
}

void player_damage(Entity* self) {
	playerData* data;

	if (!self) {
		return;
	}
	data = self->data;

	if (data->invincibility > 0) {
		return;
	}

	data->invincibility = IFRAMES; // prevent taking damage again for a while
	
	self->velocity.z = RECOIL;
	if (data->rotdir == 1) {
		self->velocity.y = -RECOIL;
	}
	else if (data->rotdir == 2){
		self->velocity.y = RECOIL;
	}

	// take damage code here
	// also include invincibility frames if there's time

	if (data->haspowerup == 0) {
		if (data->health > 0) {
			rings_dropped(self, self->position, data->health); // scatter rings everywhere
			data->health = 0; //needs ui element
		}
		else {
			player_die(self); // took dmg at 0 health so you lose!
		}
	}
	else {
		data->haspowerup = 0;
		data->fireshield = 0;
		data->bubbleshield = 0;
		data->electricityshield = 0;
		data->normalshield = 0;
	}
	

	// make rings explode everywhere too
}

void player_die(Entity* self) {
	playerData* data;

	if (!self) {
		return;
	}
	data = self->data;

	// theres no built in way of restarting a level or the game itself so just 'respawn' the player
	Mix_HaltMusic();
	Mix_PlayMusic(data->normal_music, -1);
	data->health = 0;
	data->lives -= 1;
	self->position.z = 200;
	self->position.x = 0;
	self->position.y = 0;
	self->velocity.z = 0;
}

// RING FUNCTION

void give_ring(Entity* self) {
	playerData* data;

	if (!self) {
		return;
	}
	data = self->data;

	//slog("gained ring back from dropped");
	data->health += 1;
}

// LOOP FUNCTION
// TO DO:
// read what you put in obstacle.c loop section
void player_loop(Entity* self, loopData* loop) { // for loop obstacle
	playerData* data;
	if (!self) {
		return;
	}
	data = self->data;

	if (data->currentpoint >= 10) {
		data->inloop = 0;
		return;
	}

	//slog("current point: %i", data->currentpoint);
	//slog("x, y, z positions: %.2f, %.2f, %2.f", self->position.x, self->position.y, self->position.z);

	
	if (data->rotdir == 1) {
		self->position = loop->points_backward[data->currentpoint];
	}
	else if (data->rotdir == 2) {
		self->position = loop->points_forward[data->currentpoint];
	}
	data->currentpoint += 1;
}

////////////////////////////////////
// POWER-UPS
///////////////////////////////////

void player_powerup(Entity* self, itemboxData* itembox) {
	playerData* data;
	if (!self) {
		return;
	}
	data = self->data;

	if (itembox->item == 1) {
		slog("got fire shield");
		if (data->shield) { // prevent multiple visual shields at once
			sentence_to_death(data->shield);
		}
		data->fireshield = 1;
		data->haspowerup = 1;
		data->shield = shield_spawn(self->position, self, itembox->item);

		data->bubbleshield = 0;
		data->electricityshield = 0;
		data->normalshield = 0;
	}
	else if (itembox->item == 2) {
		slog("got bubble shield");
		if (data->shield) { // prevent multiple visual shields at once
			sentence_to_death(data->shield);
		}
		data->bubbleshield = 1;
		data->haspowerup = 1;
		data->shield = shield_spawn(self->position, self, itembox->item);

		data->fireshield = 0;
		data->electricityshield = 0;
		data->normalshield = 0;
	}
	else if (itembox->item == 3) {
		slog("got electricity shield");
		if (data->shield) { // prevent multiple visual shields at once
			sentence_to_death(data->shield);
		}
		data->electricityshield = 1;
		data->haspowerup = 1;
		data->shield = shield_spawn(self->position, self, itembox->item);

		data->bubbleshield = 0;
		data->fireshield = 0;
		data->normalshield = 0;
	}
	else if (itembox->item == 4) {
		slog("got normal shield");
		if (data->shield) { // prevent multiple visual shields at once
			sentence_to_death(data->shield);
		}
		data->normalshield = 1;
		data->haspowerup = 1;
		data->shield = shield_spawn(self->position, self, itembox->item);

		data->bubbleshield = 0;
		data->fireshield = 0;
		data->electricityshield = 0;
	}
	else if (itembox->item == 5) {
		// TO DO:
		// add cool music change for its duration
		slog("got invincibility");
		if (data->shield) { // prevent multiple visual shields at once
			sentence_to_death(data->shield);
		}
		data->invincibilitypowerup = 1;
		data->invincibility = 60; // long-time (though the timer goes down faster than you would expect)
		data->shield = shield_spawn(self->position, self, itembox->item);

		data->fireshield = 0;
		data->bubbleshield = 0;
		data->electricityshield = 0;
		data->normalshield = 0;
		data->haspowerup = 0;
	}
}

// TOOL CHAIN FUNCTIONALITY
// TO DO: optimize the entity list picker so its not just a bunch of if/else statements (if possible)

void debug_think(Entity* self) {
	playerData* data;

	if (!self || !self->data) {
		return;
	}
	data = self->data;

	choose_entity(self);

	// MOVEMENT
	if (gfc_input_command_held("walkleft")) {
		self->position.y += 1 * data->debugspeedup;
	}
	if (gfc_input_command_held("walkright")) {
		self->position.y -= 1 * data->debugspeedup;
	}
	if (gfc_input_key_held("w")) {
		self->position.z += 1 * data->debugspeedup;
	}
	if (gfc_input_key_held("s")) {
		self->position.z -= 1 * data->debugspeedup;
	}
	if (gfc_input_key_held("z")) {
		self->position.x += 1 * data->debugspeedup;
	}
	if (gfc_input_key_held("x")) {
		self->position.x -= 1 * data->debugspeedup;
	}
	if (gfc_input_key_held("LSHIFT")) { // move faster
		slog("debug speed activated");
		data->debugspeedup = 5;
	}
	else if (gfc_input_key_held("LCTRL")) { // move slower
		slog("debug slow activated");
		data->debugspeedup = 0.1;
	}
	else {
		data->debugspeedup = 1; // 1x
	}
	// CONTROLS
	// debug mode toggle in player think
	if (gfc_input_key_pressed("j")) { // recenter (change to have multiple keybinds for each position or something)
		self->position.x = 0;
		self->position.y = 0;
		self->position.z = 0;
	}
	if (gfc_input_key_pressed("LEFT")) {
		// cycle left through objects
		data->entitycycle -= 1;
	}
	if (gfc_input_key_pressed("RIGHT")) {
		// cycle right through objects
		data->entitycycle += 1;
	}
	if (gfc_input_key_pressed("p")) { // save to a file for each position
		if (data->entitycycle == 0) { 
			spring_spawn(gfc_vector3d(self->position.x, self->position.y, self->position.z));
		}
		else if (data->entitycycle == 1) {
			spikes_spawn(gfc_vector3d(self->position.x, self->position.y, self->position.z));
		}
		else if (data->entitycycle == 2) {
			bomb_spawn(gfc_vector3d(self->position.x, self->position.y, self->position.z));
		}
		else if (data->entitycycle == 3) {
			v_moving_platform_spawn(gfc_vector3d(self->position.x, self->position.y, self->position.z));
		}
		else if (data->entitycycle == 4) {
			loop_spawn(gfc_vector3d(self->position.x, self->position.y, self->position.z));
		}
		else if (data->entitycycle == 5) {
			itembox_spawn(gfc_vector3d(self->position.x, self->position.y, self->position.z), 1);
		}
		else if (data->entitycycle == 6) {
			terrain_spawn(gfc_vector3d(self->position.x, self->position.y, self->position.z));
		}
	}
	if (gfc_input_command_down("giverings")) { // "h"
		data->health += 1;
	}


}

void choose_entity(Entity* self) {
	// you can add a if condition for only certain objects to be available depending on what level is activated (actually no)
	// there must definietly be a way to make this better
	playerData* data;
	if (!self || !self->data) {
		return;
	}
	data = self->data;

	if (data->entitycycle == 0) {
		self->model = gf3d_model_load("models/springs.model");
	}
	else if (data->entitycycle == 1) {
		self->model = gf3d_model_load("models/spikes.model");
	}
	else if (data->entitycycle == 2) {
		self->model = gf3d_model_load("models/shield.model"); // bomb
	}
	else if (data->entitycycle == 3) {
		self->model = gf3d_model_load("models/platform.model");
	}
	else if (data->entitycycle == 4) {
		self->model = gf3d_model_load("models/loop.model");
	}
	else if (data->entitycycle == 5) {
		self->model = gf3d_model_load("models/itembox.model"); // need a way to differentiate between each box
	}
	else if (data->entitycycle == 6) {
		self->model = gf3d_model_load("models/collisiontest.model");
	}
	else if (data->entitycycle == 7) {
		self->model = gf3d_model_load("models/enemytest.model"); // need a way to differentiate between each enemy
	}
	else if (data->entitycycle == 8) {
		self->model = gf3d_model_load("models/boss.model");
	}
	else if (data->entitycycle == 9) {
		self->model = gf3d_model_load("models/springs.model");
	}
}

/*
typedef struct {

	Entity* entityList;
	Uint32 entityMax;

}EntityManager;

void choose_entity(Entity* self) {
	static EntityManager entity_manager = { 0 };
	playerData* data;

	if (!self || !self->data) {
		return;
	}
	data = self->data;

	int i = data->entitycycle;
	for (i = 0; i < entity_manager.entityMax; i++) {
		if (!entity_manager.entityList[i]._inuse) {
			continue; // skip ones not set
		}
		data->entitycycle = &entity_manager.entityList[i];
	}
}
*/