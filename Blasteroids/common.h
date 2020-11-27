//
//  common.h
//  Blasteroids
//
//  All the common settings defined here, and also include the common allegro library needed.
//
//  Created by linxucc on 2020/11/21.
//  Copyright © 2020 lin. All rights reserved.
//

#ifndef common_h
#define common_h

#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>


/* Helper macro, translate 0-360 degree to rad */

#define DEGTORAD(x) ((x)*(ALLEGRO_PI/180.0))


/* Game settings */

// draw buffer
#define BUFFER_WIDTH 1280
#define BUFFER_HEIGHT 960

// actual screen
#define DISP_SCALE 2
#define DISP_WIDTH (BUFFER_WIDTH * DISP_SCALE)
#define DISP_HEIGHT (BUFFER_HEIGHT * DISP_SCALE)

// display frame rate (game tick)
#define GAME_FPS 30

/* For elegant keyborad reading. */
#define KEY_SEEN     1
#define KEY_RELEASED 2


/* Spaceship settings */

// each key press event, increase angle by X degree.
#define ANGLE_ACC 5
// each key press event, increase spd by X px/s
#define LINEAR_ACC 10
// in px/second
#define MAX_SPEED 150.0
// in seconds,
#define SHIP_INVICIBLE_TIME 2
// in Hz
#define SHIP_FLASH_FREQUENCY 4


/* Life counter */

#define LIFE_COUNTER_INIT_LIVES 3
#define LIFE_COUNTER_POSITION_LEFT_X 50.0
#define LIFE_COUNTER_POSTIION_LEFT_Y 100.0
#define LIFE_COUNTER_SCALE 1.5
#define LIFE_COUNTER_PADDING 25.0


/* Score */

#define SCORE_STEP 100
#define SCORE_POSITION_X 5.0
#define SCORE_POSITION_Y 5.0
#define SCORE_SCALE 5.0


/* Blast */

// in original px before scale
#define BLAST_LENGTH 10
// how wide should blast dash(line) be, in original px before scale
#define BLAST_WIDTH 1.5


/* Asteroid */

// When scale factor == 1, life is 3.
// total life is random_life * scale.
#define ASTEROID_BASE_LIFE 3
// in px/second
#define ASTEROID_MAX_SPEED 200.0
// in degree(0-360)/second
#define ASTEROID_MAX_ROT_SPEED 20
// initial number of asteroids in the screen.
#define ASTEROID_INIT_NUMBER 10
// default size(scale) range
#define ASTEROID_DEFAULT_SCALE_FACTOR_MIN 0.5
#define ASTEROID_DEFAULT_SCALE_FACTOR_MAX 3
// when hit, after split direction should change by how many degree
#define ASTEROID_DEFAULT_SPLIT_DEGREE 45
// after each hit, how small should the splited ones be
#define ASTEROID_SCALE_FACTOR_WHEN_HIT 1.5


/* Level */

// all the countdown is in seconds, control how long the overlay level message should display.
#define DEFAULT_LEVEL_COUNTDOWN 1
#define DEFAULT_START_COUNTDOWN 1
#define DEFAULT_WIN_COUNTDOWN 1


/* DEBUG mode, show console outputs */

#define VERBOSE 1
#define PRINT_DEBUG(x) (if(VERBOSE) {printf("\n%s\n", x);})


/* Some helper function goes here. */

int random_between(int lo, int hi);

float random_between_f(float lo, float hi);

int go_error(char* err_msg);

void must_init(bool test, const char *description);

void draw_text_center_overlay(ALLEGRO_FONT* font, float pos_x, float pos_y, ALLEGRO_COLOR font_color, ALLEGRO_COLOR background_color, float scale, char* text_to_draw);

void draw_level_center_overlay(ALLEGRO_FONT* font, float pos_x, float pos_y, ALLEGRO_COLOR font_color, ALLEGRO_COLOR background_color, float scale, int level);




#endif /* common_h */

