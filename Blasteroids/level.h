//
//  level.h
//  Blasteroids
//
//  Created by linxucc on 2020/11/26.
//  Copyright © 2020 lin. All rights reserved.
//

#ifndef level_h
#define level_h

#include <stdio.h>
#include "common.h"

typedef enum {
    NEW_LEVEL_NUMBER,   // display new level nubmer, center the spaceship, block input.
    LEVEL_START,    // display level start, last time in this, open input, change state.
    IN_GAME_PLAY,
    LEVEL_WIN,   // display win, when last time in this, change to next level.
    NEXT_LEVEL,     // ready to go next level.
    GAME_OVER
} GameStatus;

typedef struct {
    int level_number;
    int asteroid_total;
    GameStatus game_status;
    float count_down_of_game_status[4];
//    float display_level_number_countdown;
//    float display_start_countdown;
//    float display_win_countdown;
} Level;

/*
 Public functions
 */

int level_first_init(Level* level);

int level_go_next(Level* level);

float level_tick(Level* level);

bool is_game_over(Level* level);

int level_destroy(Level* level);



#endif /* level_h */
