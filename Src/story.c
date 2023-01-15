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

#include "story.h"

static ALLEGRO_BITMAP* comicStrip = NULL;
int comicStripW = 973;
int comicStripH = 745;




static Button exitButton;
static Button addMusicVolume;
static Button decreaseMusicVolume;
static Button addEffectVolume;
static Button decreaseEffectVolume;
static Button storyButton;
static ALLEGRO_SAMPLE_ID menuBGM;


extern ALLEGRO_SAMPLE* themeMusic;
extern ALLEGRO_SAMPLE* settingsMusic;
extern ALLEGRO_SAMPLE* PACMAN_MOVESOUND;
extern ALLEGRO_SAMPLE* PACMAN_DEATH_SOUND;
extern ALLEGRO_FONT* menuFont;
extern int fontSize;
float music_volume;
float effect_volume;
extern bool gameDone;



// The only function that is shared across files.


static void init() {
	menuBGM = play_bgm(settingsMusic, music_volume);
	exitButton = button_create(730, 20, 50, 50, "Assets/exit-game.png", "Assets/exit-game-after.png");
	comicStrip = load_bitmap("Assets/comic_strip2.png");
}



static void draw(void) {
	al_clear_to_color(al_map_rgb(0, 0, 0));

	const float scale = 0.7;
	const float offset_w = (SCREEN_W >> 1) - 0.5 * scale * comicStripW;
	const float offset_h = (SCREEN_H >> 1) - 0.5 * scale * comicStripH;

	//draw title image/*
	al_draw_scaled_bitmap(
		comicStrip,
		0, 0,
		comicStripW, comicStripH,
		offset_w, 150,
		comicStripW * scale, comicStripH * scale,
		0
	);

	drawButton(exitButton);


	al_draw_text(
		menuFont,
		al_map_rgb(255, 255, 255),
		SCREEN_W / 2.0,
		SCREEN_H - 750,
		ALLEGRO_ALIGN_CENTER,
		"STORY:"
	);

}

static void destroy() {
	stop_bgm(menuBGM);
	al_destroy_bitmap(exitButton.hovered_img);
	al_destroy_bitmap(exitButton.default_img);
	printf("Destroyed Story buttons\n");

}


static void on_mouse_down() {
	printf("INSIDE MOUSE DOWN\n");

	if (exitButton.hovered == true) {
		game_change_scene(scene_menu_create());
	}
}

static void on_mouse_move(int a, int mouse_x, int mouse_y, int f) {
	exitButton.hovered = buttonHover(exitButton, mouse_x, mouse_y);
	addMusicVolume.hovered = buttonHover(addMusicVolume, mouse_x, mouse_y);
	decreaseMusicVolume.hovered = buttonHover(decreaseMusicVolume, mouse_x, mouse_y);
	addEffectVolume.hovered = buttonHover(addEffectVolume, mouse_x, mouse_y);
	decreaseEffectVolume.hovered = buttonHover(decreaseEffectVolume, mouse_x, mouse_y);
	storyButton.hovered = buttonHover(storyButton, mouse_x, mouse_y);
}



Scene scene_story_create(void) {
	Scene scene;
	memset(&scene, 0, sizeof(Scene));
	scene.name = "Settings";
	scene.initialize = &init;
	scene.destroy = &destroy;			//DESTROY THIS LATER
	scene.on_mouse_move = &on_mouse_move;		//Create this later
	scene.on_mouse_down = &on_mouse_down;
	scene.draw = &draw;
	// TODO: Register more event callback functions such as keyboard, mouse, ...
	game_log("Settings story created");
	return scene;
}