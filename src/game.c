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
#include "terraintest.h"

extern int __DEBUG;

static int _done = 0;
static Uint32 frame_delay = 33;
static float fps = 0;

void parse_arguments(int argc,char *argv[]);
void game_frame_delay();

void exitGame()
{
    _done = 1;
}

void draw_origin()
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
}

// ME: *** EVERYTHING is dependant on this function, this is how the program starts (it's the main function obviously)

int main(int argc,char *argv[]) 
{
    //entity
    Entity ent; // ME: idk what this was supposed to be for
    //local variables
    Model *sky, *collisiontest;
    GFC_Matrix4 skyMat, collisiontestMat;
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
    //gfc_config_def_init(); idk what this was for
    gfc_action_init(1024);
    //gf3d init
    gf3d_vgraphics_init("config/setup.cfg"); // check setup.cfg - you can enable/disable stuff like resolution, fps, developer debug features, and the game name
    gf3d_materials_init();
    gf2d_font_init("config/font.cfg");
    gf2d_actor_init(1000); // ME: this and draw_manager will be how much entities can be allowed at once, but draw_manager_init is for 2D only
    gf3d_draw_init();//3D
    gf2d_draw_manager_init(1000);//2D

    entity_system_init(1000);
    
    //game init
    srand(SDL_GetTicks()); // random tick thing
    slog_sync();

    //game setup
    gf2d_mouse_load("actors/mouse.actor");
    sky = gf3d_model_load("models/sky.model"); // the skybox is a model (a big cylinder)
    gfc_matrix4_identity(skyMat);
    //dino = gf3d_model_load("models/dino.model"); // you can use blender to make your own models (simple ones at least) 
    //gfc_matrix4_identity(dinoMat);
    collisiontest = gf3d_model_load("models/collisiontest.model");
    gfc_matrix4_identity(collisiontestMat);

    //camera

    // ME: entity code
    /*/ i have no idea what was supposed to go here
    if (ent._inuse) {
        Entity* self;
        self = entity_new();

        self->model = gf3d_model_load("models/dino.model");
        self->think = dino_think;
        //self->position = ????? is this even necessary 
        // ent = dinosaur model path (self->model = gf3d_model_load("models/dino.model");)
        // ent think = something (you can make it roatete or something)
        // dino.h and dino.c are where this function can be placed in instead and things relating to this
    }*/

    // ME: this will most likely be changed dramatically as the game develops, since it's just a freecam for now (as it is when the project is first compiled)
    gf3d_camera_set_scale(gfc_vector3d(1,1,1));
    gf3d_camera_set_position(gfc_vector3d(15,-15,10));
    gf3d_camera_look_at(gfc_vector3d(0,0,0),NULL);
    gf3d_camera_set_move_step(0.2);
    gf3d_camera_set_rotate_step(0.05);
    
   // gf3d_camera_enable_free_look(1);
    dino_spawn(gfc_vector3d(0, 0, 0)); // spawn an entity instead 
    terrain_spawn(gfc_vector3d(0, 0, -100));
    
    //windows

    // main game loop    
    while(!_done) // ME: self explanatory - updates the primary functions constantly as long as the game is open
    {
        gfc_input_update();
        gf2d_mouse_update();
        gf2d_font_update();

        entity_think_all(); // me: create this
        entity_update_all(); // me: uhh

        //camera updaes
        gf3d_camera_controls_update();
        gf3d_camera_update_view();
        gf3d_camera_get_view_mat4(gf3d_vgraphics_get_view_matrix());

        gf3d_vgraphics_render_start();

        //3D draws
        
        gf3d_model_draw_sky(sky,skyMat,GFC_COLOR_WHITE);
        /*gf3d_model_draw(
            dino, // dinosaur model location, if y  ou delete it it wont appear in the game UNLESS YOU HVAE ANOTHER WAY OF SAPWNING IT
            dinoMat,
            GFC_COLOR_WHITE,
            NULL,
            0);*/
        entity_draw_all(); // because this is already done in entity.c
        draw_origin();
        //2D draws
        gf2d_mouse_draw();
        gf2d_font_draw_line_tag("alt+f4 to exit",FT_H1,GFC_COLOR_WHITE, gfc_vector2d(10,10));
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
/*eol@eof*/
