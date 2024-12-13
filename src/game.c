#include <SDL.h>            

#include "simple_json.h"
#include "simple_logger.h"

#include "gfc_input.h"
#include "gfc_config_def.h"
#include "gfc_vector.h"
#include "gfc_matrix.h"
#include "gfc_audio.h"
#include "gfc_string.h"
#include "gfc_actions.h"

#include "gf2d_sprite.h"
#include "gf2d_font.h"
#include "gf2d_draw.h"
#include "gf2d_actor.h"
#include "gf2d_mouse.h"

#include "gf3d_vgraphics.h"
#include "gf3d_pipeline.h"
#include "gf3d_swapchain.h"
#include "gf3d_model.h"
#include "gf3d_camera.h"
#include "gf3d_texture.h"
#include "gf3d_draw.h"

#include "entity.h"
#include "player.h"
#include "terrain.h"
#include "enemy.h"
#include "rings.h"
#include "obstacles.h"
#include "UI.h"
#include "boss.h"
#include "mainmenu.h"

extern int __DEBUG;

static int _done = 0;
static Uint32 frame_delay = 33;
static float fps = 0;

int gameStarted = 0;
int debugmode = 0;

void parse_arguments(int argc,char *argv[]);
void game_frame_delay();
void load_game();
void load_level();

void exitGame()
{
    _done = 1;
}
// i dont think ill ever need this again, but this is the code for the 3 axis lines
/*void draw_origin() 
{
    gf3d_draw_edge_3d(
        gfc_edge3d_from_vectors(gfc_vector3d(-100,0,0),gfc_vector3d(100,0,0)),
        gfc_vector3d(0,0,0),gfc_vector3d(0,0,0),gfc_vector3d(1,1,1),0.1,gfc_color(1,0,0,1));
    gf3d_draw_edge_3d(
        gfc_edge3d_from_vectors(gfc_vector3d(0,-100,0),gfc_vector3d(0,100,0)),
        gfc_vector3d(0,0,0),gfc_vector3d(0,0,0),gfc_vector3d(1,1,1),0.1,gfc_color(0,1,0,1));
    gf3d_draw_edge_3d(
        gfc_edge3d_from_vectors(gfc_vector3d(0,0,-100),gfc_vector3d(0,0,100)),
        gfc_vector3d(0,0,0),gfc_vector3d(0,0,0),gfc_vector3d(1,1,1),0.1,gfc_color(0,0,1,1));
}*/

// ME: *** EVERYTHING is dependant on this function, this is how the program starts (it's the main function obviously)

int main(int argc,char *argv[]) 
{
    //local variables
    Model* sky;
    GFC_Matrix4 skyMat;

    //initializtion    
    /* ME:
    this is what logs crashes to see what went wrong or something
    it relies on the "slog" functions
    go to void init_logger and look around that area for how it works
        - append mode parameter changes something i forgor but its something to do with debugging (check the video)
    this can be used in every file by copying and pasting it, but you would have to include the .h files for the logger like simple_logger (i think thats the only one)
    */
    parse_arguments(argc,argv);
    init_logger("gf3d.log",0); 
    slog("gf3d begin");

    //gfc init
    gfc_input_init("config/input.cfg"); // check input.cfg - it has premade controls to play the game and do game stuff (they can be changed/deleted)
    // music
    gfc_audio_init(100, 10, 0, 10, 1, 0); // hmm
    gfc_config_def_init(); // this was off before, bt im leaving it on in case i go crazy if i try to use this and wonder why it doesnt work
    gfc_action_init(1024);
    //gf3d init
    gf3d_vgraphics_init("config/setup.cfg"); // check setup.cfg - you can enable/disable stuff like resolution, fps, developer debug features, and the game name
    gf3d_materials_init();
    gf2d_font_init("config/font.cfg");
    gf2d_actor_init(1000); // ME: this and draw_manager will be how much entities can be allowed at once, but draw_manager_init is for 2D only
    gf3d_draw_init();//3D
    gf2d_draw_manager_init(1000);//2D

    // menu init
    prepare_menu_UI();

    entity_system_init(10000); // was 1000, and this is way too much but its a band-aid solution
    
    //game init
    srand(SDL_GetTicks()); // random tick thing
    slog_sync();

    //game setup
    gf2d_mouse_load("actors/mouse.actor");
    sky = gf3d_model_load("models/sky.model"); // the skybox is a model (a big cylinder)
    gfc_matrix4_identity(skyMat);

    // i put the music in player.c for easier management
    //gfc_sound_load
    //gfc_sound_play(music, 1, 1, -1, -1);
    

    //camera
    // ME: this will most likely be changed dramatically as the game develops, since it's just a freecam for now (as it is when the project is first compiled)
    // ME FROM THE FUTURE: turns out i didnt need to (for now?)
    gf3d_camera_set_scale(gfc_vector3d(1,1,1));
    gf3d_camera_set_position(gfc_vector3d(15,-15,10));
    gf3d_camera_look_at(gfc_vector3d(0,0,0),NULL);
    gf3d_camera_set_move_step(0.2);
    gf3d_camera_set_rotate_step(0.05);
    
   // gf3d_camera_enable_free_look(1);
    /*// time (not used for anything get but good to have)
    float deltatime = 0;
    game_frame_delay(&deltatime);*/
    // never mind
    
    
    //windows

    // main game loop    
    // ME: self explanatory - updates the primary functions constantly as long as the game is open
    while(!_done) {

        gfc_input_update();
        gf2d_mouse_update();
        gf2d_font_update();
        // new stuff
        entity_think_all();
        entity_update_all();

        //camera updaes
        gf3d_camera_controls_update();
        gf3d_camera_update_view();
        gf3d_camera_get_view_mat4(gf3d_vgraphics_get_view_matrix());

        gf3d_vgraphics_render_start();

        //3D draws
        
        gf3d_model_draw_sky(sky,skyMat,GFC_COLOR_WHITE);
        entity_draw_all(); // entity.c stuff

        //2D draws
        //gf2d_mouse_draw(); // have this show only if navigating menu (or jst make menu be keybind based i gues)
        //gf2d_font_draw_line_tag("alt+f4 to exit",FT_H1,GFC_COLOR_WHITE, gfc_vector2d(10,10));
        // DRAW UI

        draw_UI();

        if (gameStarted == 0) {
            draw_menu(); // show if game didnt start yet
            load_game();
        }
        
        gf3d_vgraphics_render_end();
        if (gfc_input_command_down("exit"))_done = 1; // exit condition
        game_frame_delay();
    }    
    vkDeviceWaitIdle(gf3d_vgraphics_get_default_logical_device());    
    //cleanup
    slog("gf3d program end");
    exit(0);
    slog_sync();
    return 0;
}

void parse_arguments(int argc,char *argv[])
{
    int a;

    for (a = 1; a < argc;a++)
    {
        if (strcmp(argv[a],"--debug") == 0)
        {
            __DEBUG = 1;
        }
    }    
}

void game_frame_delay()
{
    Uint32 diff;
    static Uint32 now;
    static Uint32 then;
    then = now;
    slog_sync();// make sure logs get written when we have time to write it
    now = SDL_GetTicks();
    diff = (now - then);
    if (diff < frame_delay)
    {
        SDL_Delay(frame_delay - diff);
    }
    fps = 1000.0/MAX(SDL_GetTicks() - then,0.001);
//     slog("fps: %f",fps);
}

void load_game() {
    menuState currentState = get_menu();

    if (currentState == NORMAL) {
    // LOADING SCREEN STUFF GOES HERE

    // maybe make music play for the first time the game is started until the menu ends or the music ends
    // 
    // ENTITY SPAWNING
    // z value -140 - -180 is close enough to ground (collisiontest) and within the player's reach
    // these are all hard-coded (make data-coded for final)

    // everything below here needs to be moved into a def file somehow
    // make sure everything is data coded not hard coded
    // i think i made notes on google doc how it should look
    // also check the master branch?
    // 
    // PLAYER SPAWN
        player_spawn(gfc_vector3d(0, 500, -150));

        // TERRAIN SPAWN
        terrain_spawn(gfc_vector3d(0, -9000, -200)); // the ground has a LOT to go off of
        //test_spawn(gfc_vector3d(-1000, 0, -750)); // the model never loads for some reason and i dont feel like figuring it out anymore

        // OBSTACLE SPAWNS
        spikes_spawn(gfc_vector3d(0, -300, -155));
        spring_yellow_spawn(gfc_vector3d(0, -350, -160));
        v_moving_platform_spawn(gfc_vector3d(0, -450, -150));
        loop_spawn(gfc_vector3d(0, -650, -165)); // x = 20 except collisions break when you do that for some reason

        // ITEMBOX SPAWNS (OBSTACLE SUBCATEGORY)
        itembox_spawn(gfc_vector3d(0, -800, -165), 1);
        itembox_spawn(gfc_vector3d(0, -825, -165), 2);
        itembox_spawn(gfc_vector3d(0, -850, -165), 3);
        itembox_spawn(gfc_vector3d(0, -875, -165), 4);
        itembox_spawn(gfc_vector3d(0, -900, -165), 5);

        // ENEMY SPAWNS
        generic_enemy_spawn(gfc_vector3d(0, -100, 0));
        projectile_enemy_spawn(gfc_vector3d(0, 100, -140));
        flying_enemy_spawn(gfc_vector3d(0, 200, -140));
        spike_enemy_spawn(gfc_vector3d(0, 350, -140));
        bomb_enemy_spawn(gfc_vector3d(0, 400, -100));

        boss_spawn(gfc_vector3d(0, -1250, -155));

        // RING ENTITY LIST (WILL MOST LIKELY BE A LOT)
        // (first 3: grounded)
        rings_spawn(gfc_vector3d(0, -50, -155), 0);
        rings_spawn(gfc_vector3d(0, -40, -155), 0);
        rings_spawn(gfc_vector3d(0, -30, -155), 0);
        // (second 3: aerial)
        rings_spawn(gfc_vector3d(0, 30, -135), 0);
        rings_spawn(gfc_vector3d(0, 40, -135), 0);
        rings_spawn(gfc_vector3d(0, 50, -135), 0);
        //

        gameStarted = 1;
        slog("GAME STARTED");
    }
    else if (currentState == OBJECTIVE) {
        gameStarted = 1;
        slog("GAME STARTED");
    }
    else if (currentState == DEBUG) {

        player_spawn(gfc_vector3d(0, 0, 0));
        load_level();
        //terrain_spawn(gfc_vector3d(0, 0, -100));

        debugmode = 1;
        gameStarted = 1;

        slog("GAME STARTED");
    }
}
// dont forget to put empty function at the top of this file if you change the parameters
// look in player.c and toolchain.c to see how it saves data
void load_level(/* should take filename, but ill do that later */) { // json file analyzer
    SJson* file = sj_load("def/levels/toolchain.json"); // placeholder
 
    if (!file) {
        slog("could not open file");
        return;
    }

    SJson* parse = sj_object_get_value(file, "entitylist");

    if (!parse) {
        slog("did not find any object name");
        return;
    }

    if (!sj_is_array(parse)) {
        slog("value of object is not an array");
        return;
    }

    for (int i = 0; i < sj_array_get_count(parse); i++) {
        SJson* list = sj_array_get_nth(parse, i);

        char* name = sj_object_get_string(list, "name");

        if (!name) {
            slog("no 'name' found in json");
        }

        char* type = sj_object_get_string(list, "type");

        if (!type) {
            slog("no 'type' found in json");
        }

        float x = 0, y = 0, z = 0;
        SJson* position = sj_object_get_value(list, "position");


        if (position) {
            SJson* x_pos = sj_array_get_nth(position, 0);
            sj_get_float_value(x_pos, &x);

            SJson* y_pos = sj_array_get_nth(position, 1);
            sj_get_float_value(y_pos, &y);

            SJson* z_pos = sj_array_get_nth(position, 2);
            sj_get_float_value(z_pos, &z);
        }
        else {
            x = 0, y = 0, z = 0; // default
            slog("no specific position found, all assigned to 0 by default");
        }

        GFC_Vector3D parsedposition = gfc_vector3d(x, y, z);

        if (!type) {
            slog("no type found, spawning default entity");
        }

        if (strcmp(name, "terrain") == 0) {
            if (strcmp(type, "floor") == 0) {
                terrain_spawn(parsedposition);
            }
            slog("terrain spawned (x: %.2f, y: %.2f, z: %.2f)", x, y, z);
        }
        if (strcmp(name, "spring") == 0) {
            if (strcmp(type, "yellow") == 0) {
                spring_yellow_spawn(parsedposition);
            }
            slog("obstacle spawned (x: %.2f, y: %.2f, z: %.2f)", x, y, z);
        }

    }
}

/*eol@eof*/
