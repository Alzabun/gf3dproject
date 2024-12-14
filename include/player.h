#ifndef __PLAYER_H__
#define __PLAYER_H__
		
#include "entity.h"
#include "obstacles.h"
#include "terrain.h"
#include "enemy.h"
#include "rings.h"
#include "shield.h"
#include "mainmenu.h"
#include "projectile.h"

#include "gfc_audio.h"
#include "entitykiller.h"
#include "toolchain.h"

typedef struct {
	// GENERIC
	GFC_Vector3D position; // self explanatory

	// UI VARIABLES
	float speed_y; 
	float speed_z; 
	//float speed_x; not really important yet
	float deltatime; // player is god of time confirmed????
	float position_x;
	float position_y;
	float position_z;

	// GAME
	int lives;

	// CONTROLS
	int jumpTime; // goes up by a number each frame or whatever
	float storedvelocity; // for spindash speed

	// BOOLEANS
	int airborne; // 0 = no, 1 = yes
	int spindash; // 0 = no, 1 = yes
	int inball; // 0 = no, 1 = yes
	int rotdir; // 1 = left, 2 = right
	int health; // amount of rings
	float invincibility; // either for i-frames or power-ups
	int onPlatform; // 0 = no, 1 = yes | this is to prevent not being able to jump off a platform

	// LOOP MANAGEMENT
	int inloop; // 0 = no, 1 = yes
	int currentpoint; // find amount of points from obstacles.c loop section
	loopData* thisloop; // automatic waypoints

	// POWERUP MANAGEMENT
	itemboxData* thispowerup;
	Entity* shield;
	int fireshield; // 0 = no, 1 = yes
	int bubbleshield; // 0 = no, 1 = yes
	int bubblebounce; // 0 = no, 1 = yes
	int electricityshield; // 0 = no, 1 = yes
	int doublejumped; // 0 = no, 1 = yes
	int normalshield; // 0 = no, 1 = yes
	int invincibilitypowerup; // 0 = no, 1 = yes
	int haspowerup; // 0 = no, 1 = yes

	// TERRAIN CONDITION MANAGEMENT
	int inSand; // 0 = no, 1 = yes
	int inWater; // 0 = no, 1 = yes
	int inOil; // 0 = no, 1 = yes
	int inIce; // 0 = no, 1 = yes
	float oxygen;

	// MUSIC
	Mix_Music* normal_music;
	Mix_Music* boss_music;
	Mix_Music* wintheme;
	Mix_Chunk* jump; // should be very similar to how music was implemented.. or i could just implement sounds as music but im not sure if thall cause memory issues

	// LEVEL ENDING
	int reachedgoal; // 0 = no, 1 = yes
	GFC_Vector3D lockedcamera;
	// boss check for ui
	int killedboss;

	//debug
	int debugmode; // 0 = no, 1 = yes
	int indebug; // 0 = no, 1 = yes
	float debugspeedup; // if no: 1x speed, if yes: multiplier

	int entitycycle; 
	int entitylist;

}playerData;

void player_think(Entity* self);
void player_update(Entity* self);
void player_free(Entity* self);
void player_touch(Entity* self, Entity* other);
void player_camera(Entity* self);
void player_damage(Entity* self);
void player_die(Entity* self);
void player_loop(Entity* self, loopData* loop);
void player_powerup(Entity* self, itemboxData* itembox);

// TOOL CHAIN SECTION

void debug_think(Entity* self);
void debug_place(Entity* self);
void choose_entity(Entity* self);

Entity* player_spawn(GFC_Vector3D position);

#endif