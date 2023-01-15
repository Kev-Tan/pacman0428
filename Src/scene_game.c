#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <string.h>
#include "game.h"
#include "shared.h"
#include "utility.h"
#include "scene_game.h"
#include "scene_menu.h"
#include "pacman_obj.h"
#include "scene_ending.h"
#include "ghost.h"
#include "map.h"
#include "scene_win.h"


int game_main_Score = 0;
// [HACKATHON 2-0]
// Just modify the GHOST_NUM to 1
#define GHOST_NUM 4
/* global variables*/
extern const uint32_t GAME_TICK_CD;
extern uint32_t GAME_TICK;
ALLEGRO_TIMER* game_tick_timer;
extern ALLEGRO_SAMPLE* battleMusic;
bool game_over = false;
bool win = false;

/* Internal variables*/
static ALLEGRO_TIMER* speed_up_timer;
static ALLEGRO_SAMPLE_ID menuBGM;
static const int power_up_duration = 10;
static Pacman* pman;
static Map* basic_map;
static Ghost** ghosts;
bool debug_mode = false;
bool cheat_mode = false;

/* Declare static function prototypes */
static void init(void);
static void step(void);
static void checkItem(void);
static void status_update(void);
static void update(void);
static void draw(void);
static void printinfo(void);
//static void destroy(void);
static void on_key_down(int key_code);
static void on_mouse_down(void);
static void render_init_screen(void);
static void draw_hitboxes(void);
static int retrieveScore(void); 
int vulnerableTime;
static void init(void) {
    win = false;
    game_over = false;
    stop_bgm(menuBGM);
    menuBGM = play_bgm(battleMusic, music_volume);

    game_main_Score = 0;
    game_over = false;
    // create map
    basic_map = create_map("Assets/map_nthu.txt");
    // [TODO]
    // Create map from .txt file and design your own map !!
    // basic_map = create_map("Assets/map_nthu.txt");
    if (!basic_map) {
        game_abort("error on creating map");
    }
    // create pacman
    pman = pacman_create();
    if (!pman) {
        game_abort("error on creating pacamn\n");
    }

    // allocate ghost memory
    // [HACKATHON 2-1]
    // TODO: Allocate dynamic memory for ghosts array.

    ghosts = (Ghost**)malloc(sizeof(Ghost) * GHOST_NUM);

    if (!ghosts) {
        game_log("We haven't create any ghosts!\n");
    }
    else {
        // [HACKATHON 2-2]
        // TODO: create a ghost.
        // Try to look the definition of ghost_create and figure out what should be placed here.
        for (int i = 0; i < GHOST_NUM; i++) {

            game_log("creating ghost %d\n", i);
            ghosts[i] = ghost_create(i);
            if (!ghosts[i])
                game_abort("error creating ghost\n");
        }
    }
    GAME_TICK = 0;

    render_init_screen();
    power_up_timer = al_create_timer(1.0f); // 1 tick / sec
    speed_up_timer = al_create_timer(1.0f);;
    if (!power_up_timer)
        game_abort("Error on create timer\n");
    return;
}

static void step(void) {
    if (pman->objData.moveCD > 0)
        pman->objData.moveCD -= pman->speed;
    for (int i = 0; i < GHOST_NUM; i++) {
        // important for movement
        if (ghosts[i]->objData.moveCD > 0)
            ghosts[i]->objData.moveCD -= ghosts[i]->speed;
    }
}
static void checkItem(void) {
    int Grid_x = pman->objData.Coord.x, Grid_y = pman->objData.Coord.y;
    if (Grid_y >= basic_map->row_num - 1 || Grid_y <= 0 || Grid_x >= basic_map->col_num - 1 || Grid_x <= 0)
        return;
    // [HACKATHON 1-3]
    // TODO: check which item you are going to eat and use pacman_eatItem to deal with it.
    for (int i = 0; i < GHOST_NUM; i++) {
        if (vulnerableTime > power_up_duration && ghosts[i]->status==FLEE) {
            ghost_toggle_FLEE(ghosts[i], false);
            printf("Power up finish\n");
        }
    }

    if (al_get_timer_count(speed_up_timer) > power_up_duration && pman->speed ==4) {
        printf("Reset original speed\n");
        pman->speed = 2;
    }

    for (int i = 0; i < GHOST_NUM; i++) {
        vulnerableTime = al_get_timer_count(power_up_timer);
        if (vulnerableTime > power_up_duration && ghosts[i]->status == FLEE) {
            // printf("%d", al_get_timer_count(power_up_timer));
             //al_stop_timer(power_up_timer);
            printf("Change to normal");
            ghost_toggle_FLEE(ghosts[i], false);
            // al_set_timer_count(power_up_timer, 0);
             //ghost_toggle_FLEE(ghosts[i], false);
             //ghosts[i]->status = FREEDOM;
            printf("Power up finish\n");
        }
    }


    switch (basic_map->map[Grid_y][Grid_x])
    {
    case '.':
        pacman_eatItem(pman, '.');
        basic_map->beansCount--;   //Beans count decreases as we eat
        game_main_Score += 10;        //Score increases everytime we eat
        break;
        //The count of the beans will decrease by one in case it's a '.'

    case 'P':
        pacman_eatItem(pman, 'P'); 
        al_set_timer_count(power_up_timer, 0);
        al_start_timer(power_up_timer);
        game_main_Score += 20;        
        for (int i = 0; i < GHOST_NUM; i++) {
                ghost_toggle_FLEE(ghosts[i], true);
        }
        printf("Power bean is eaten\n");
      
        break;
        //The count of the beans will decrease by one in case it's a '.'
    case 'U':
        pacman_eatItem(pman, 'U');
        al_set_timer_count(speed_up_timer, 0);
        al_start_timer(speed_up_timer);
        game_main_Score += 20;
        pman->speed = 4;
        printf("Speed bean is eaten\n");
        break;
    case 'X':
        pacman_eatItem(pman, 'X');
        game_main_Score += 20;
        pman->objData.Coord.x = 24;
        pman->objData.Coord.y = 24;
        printf("Teleport bean is eaten\n");
        break;
    default:
        break;
    }

    // [HACKATHON 1-4]
    // erase the item you eat from map
    // be careful no erasing the wall block.

    basic_map->map[Grid_y][Grid_x] = ' ';

}


static void status_update(void) {
    for (int i = 0; i < GHOST_NUM; i++) {
        if (ghosts[i]->status == GO_IN)
            continue;
        // [TODO]
        //use getDrawArea(..., GAME_TICK_CD) and RecAreaOverlap(..., GAME_TICK_CD) functions to detect
        // if pacman and ghosts collide with each other.
        // And perform corresponding operations.
        // [NOTE]
        // You should have some branch here if you want to implement power bean mode.
        // Uncomment Following Code


        //Create two area based on the area of pacman and ghosts which cause pacman to die in event of colliison
        RecArea areaOne = getDrawArea(pman->objData, GAME_TICK_CD);
        RecArea areaTwo = getDrawArea(ghosts[i]->objData, GAME_TICK_CD);
        bool collide = RecAreaOverlap(areaOne, areaTwo);

   

        if (basic_map->beansCount==0)
        {   
           // al_rest(1.0);
           // pacman_die();
            stop_bgm(menuBGM);
            game_over = true;
            win = true;
            game_log("You ate all the beans!\n");
            game_change_scene(scene_win_create());
            break;
        }

        if (collide) {
            printf("Collision between ghosts and pacman, change to go in");
            ghost_collided(ghosts[i]);
        }
 

        if(!cheat_mode && collide && ghosts[i]->status == FREEDOM)
        {
         game_log("collide with ghost\n");
        // game_log("Score is %d\n", game_main_Score);
          stop_bgm(menuBGM);
         al_rest(1.0);
         pacman_die();
         game_over = true;
         game_log("Number of beans is %d\n", basic_map->beansCount);
         break;
        }

        
    }
}

static int retrieveScore(void) {
    return game_main_Score;
}

static void update(void) {
    vulnerableTime = al_get_timer_count(power_up_timer);
    if (game_over) {
         // [TODO]
         //start pman->death_anim_counter and schedule a game-over event (e.g change scene to menu) after Pacman's death animation finished
         //game_change_scene(...);
        printf("DIED!\n");
        int deathTime = al_get_timer_count(pman->death_anim_counter);
        printf("Final score is %d\n", game_main_Score);
          if (deathTime % 120 < 130 && deathTime % 120 > 118) {
         game_change_scene(scene_ending_create());
    }
        //game_change_scene(scene_menu_create());

        return;
    }
     if (win) {
         game_change_scene(scene_win_create());
    }

    step();
    checkItem();
    status_update();
    pacman_move(pman, basic_map);
    for (int i = 0; i < GHOST_NUM; i++)
        ghosts[i]->move_script(ghosts[i], basic_map, pman);
}


static void draw(void) {

    al_clear_to_color(al_map_rgb(0, 0, 0));

    al_draw_textf(
        menuFont,
        al_map_rgb(255, 255, 255),
        SCREEN_W / 6,
        SCREEN_H - 100,
        ALLEGRO_ALIGN_CENTER,
        "%d",
        game_main_Score
    );

    // [TODO]
    // Draw scoreboard, something your may need is sprinf();
    /*
     al_draw_text(...);
    */

    draw_map(basic_map);

    pacman_draw(pman);
    if (game_over)
        return;
    // no drawing below when game over
    for (int i = 0; i < GHOST_NUM; i++)
        ghost_draw(ghosts[i]);

    //debugging mode
    if (debug_mode) {
        draw_hitboxes();
    }

}

static void draw_hitboxes(void) {
    RecArea pmanHB = getDrawArea(pman->objData, GAME_TICK_CD);
    al_draw_rectangle(
        pmanHB.x, pmanHB.y,
        pmanHB.x + pmanHB.w, pmanHB.y + pmanHB.h,
        al_map_rgb_f(1.0, 0.0, 0.0), 2
    );

    for (int i = 0; i < GHOST_NUM; i++) {
        RecArea ghostHB = getDrawArea(ghosts[i]->objData, GAME_TICK_CD);
        al_draw_rectangle(
            ghostHB.x, ghostHB.y,
            ghostHB.x + ghostHB.w, ghostHB.y + ghostHB.h,
            al_map_rgb_f(1.0, 0.0, 0.0), 2
        );
    }

}

static void printinfo(void) {
    game_log("pacman:\n");
    game_log("coord: %d, %d\n", pman->objData.Coord.x, pman->objData.Coord.y);
    game_log("PreMove: %d\n", pman->objData.preMove);
    game_log("NextTryMove: %d\n", pman->objData.nextTryMove);
    game_log("Speed: %f\n", pman->speed);
}


static void destroy(void) {
    stop_bgm(menuBGM);
    pacman_destory(pman);
    delete_map(basic_map);
    
    // [TODO]
     //free map array, Pacman and ghosts
    
    for (int i = 0; i < GHOST_NUM; i++) {
        ghost_destory(ghosts[i]);
    }

}

static void on_key_down(int key_code) {
    printf("%d %d", pman->objData.Coord.x, pman->objData.Coord.y);
    switch (key_code)
    {
        // [HACKATHON 1-1]
        // TODO: Use allegro pre-defined enum ALLEGRO_KEY_<KEYNAME> to controll pacman movement
        // we provided you a function pacman_NextMove to set the pacman's next move direction.

    case ALLEGRO_KEY_W:
        pacman_NextMove(pman, UP);
        break;
    case ALLEGRO_KEY_A:
        pacman_NextMove(pman, LEFT);
        break;
    case ALLEGRO_KEY_S:
        pacman_NextMove(pman, DOWN);
        break;
    case ALLEGRO_KEY_D:
        pacman_NextMove(pman, RIGHT);
        break;
    case ALLEGRO_KEY_C:
        cheat_mode = !cheat_mode;
        if (cheat_mode)
            printf("cheat mode on\n");
        else
            printf("cheat mode off\n");
        break;
    case ALLEGRO_KEY_G:
        debug_mode = !debug_mode;
        break;

    default:
        break;
    }

}

static void on_mouse_down(void) {
    // nothing here

}

static void render_init_screen(void) {
    al_clear_to_color(al_map_rgb(0, 0, 0));

    draw_map(basic_map);
    pacman_draw(pman);
    for (int i = 0; i < GHOST_NUM; i++) {
        ghost_draw(ghosts[i]);
    }

    al_draw_text(
        menuFont,
        al_map_rgb(255, 255, 0),
        400, 400,
        ALLEGRO_ALIGN_CENTER,
        "READY!"
    );

    al_flip_display();
    al_rest(2.0);

}
// Functions without 'static', 'extern' prefixes is just a normal
// function, they can be accessed by other files using 'extern'.
// Define your normal function prototypes below.

// The only function that is shared across files.
Scene scene_main_create(void) {
    Scene scene;
    memset(&scene, 0, sizeof(Scene));
    scene.name = "Start";
    scene.initialize = &init;
    scene.update = &update;
    scene.draw = &draw;
   // scene.destroy = &destroy;
    scene.on_key_down = &on_key_down;
    scene.on_mouse_down = &on_mouse_down;
    // TODO: Register more event callback functions such as keyboard, mouse, ...
    game_log("Start scene created");
    return scene;
}
