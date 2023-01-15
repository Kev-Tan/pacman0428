#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_native_dialog.h>
#include <math.h>
#include "scene_menu_object.h"
#include "scene_settings.h"
#include "scene_game.h"
#include "scene_menu.h"
#include "utility.h"
#include "shared.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>



static Button exitButton;
static ALLEGRO_SAMPLE_ID menuBGM;

// The only function that is shared across files.
extern ALLEGRO_SAMPLE* losingMusic;

static void init() {
	stop_bgm(menuBGM);
	menuBGM = play_bgm(losingMusic, music_volume);
	exitButton = button_create(730, 20, 50, 50, "Assets/exit-game.png", "Assets/exit-game-after.png");
}

static void draw(void) {
	al_clear_to_color(al_map_rgb(0, 0, 0));

	al_draw_text(
		menuFont,
		al_map_rgb(255, 255, 255),
		SCREEN_W / 2,
		SCREEN_H - 450,
		ALLEGRO_ALIGN_CENTER,
		"GAME OVER"
	);

	al_draw_text(
		menuFont,
		al_map_rgb(255, 255, 255),
		SCREEN_W / 2,
		SCREEN_H - 350,
		ALLEGRO_ALIGN_CENTER,
		"YOU DID NOT SAVE HIS CODE"
	);

	al_draw_textf(
		menuFont,
		al_map_rgb(255, 255, 255),
		SCREEN_W / 2,
		SCREEN_H - 250,
		ALLEGRO_ALIGN_CENTER,
		"%d",
		game_main_Score
	);

	drawButton(exitButton);
}

static void destroy() {
	stop_bgm(menuBGM);
	al_destroy_bitmap(exitButton.hovered_img);
	al_destroy_bitmap(exitButton.default_img);
	printf("Destroyed Ending Scene Buttons\n");
}

static void on_mouse_down() {
	printf("INSIDE MOUSE DOWN\n");
	game_change_scene(scene_menu_create());
}

static void on_mouse_move(int a, int mouse_x, int mouse_y, int f) {
	//	[HACKATHON 3-7]
	//	TODO: Update button's status(hovered), and utilize the function `pnt_in_rect`, which you just implemented
	//	Uncomment and fill the code below	 
	exitButton.hovered = buttonHover(exitButton, mouse_x, mouse_y);
	//	 btnSettings.hovered = ???(btnSettings, mouse_x, mouse_y);
}



Scene scene_ending_create(void) {
	Scene scene;
	memset(&scene, 0, sizeof(Scene));
	scene.name = "Settings";
	scene.initialize = &init;
	scene.destroy = &destroy;			//DESTROY THIS LATER
	scene.on_mouse_move = &on_mouse_move;		//Create this later
	scene.on_mouse_down = &on_mouse_down;
	scene.draw = &draw;
	// TODO: Register more event callback functions such as keyboard, mouse, ...
	game_log("Settings scene created");
	return scene;
}