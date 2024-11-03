#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "entity.h"
#include "obstacles.h"
#include "terraintest.h"
#include "enemy.h"
#include "rings.h"
#include "shield.h"

#include "gfc_audio.h"

typedef struct {
	// GENERIC
	GFC_Vector3D position; // self explanatory

	// UI VARIABLES
	float speed_y; 
	float speed_z; 
	//float speed_x; not really important yet
	float deltatime; // player is god of time confirmed????

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

	// MUSIC
	Mix_Music* normal_music;
	Mix_Music* boss_music;

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

Entity* player_spawn(GFC_Vector3D position);

#endif