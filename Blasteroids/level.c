//
//  level.c
//  Blasteroids
//
//  Created by linxucc on 2020/11/26.
//  Copyright © 2020 lin. All rights reserved.
//

#include "level.h"

/*
 Level, when you clear all the asteroids in this level, you move to next, with more asteroids...
 
 Level controls main gama flow, it contains control information about wether or not to display level number, win, fail, start or not...
 
 For each level, the sequence is:
    Spaceship init, put it in center, new asteroids init
    Display LEVEL X
    Display START
    Fire is open (space bar event ready to response)
    ...Game Play... if Game Over -->> goto Game over state, and never back.
    Display YOU WIN!
    Fire is on hold.
 
 It controls draw what, and wether or not respond to keyboard input. Also when to Put new asteroids and reposition space ship
 
 */



int _level_asteroids_number_calculator(int level_number)
{
    // put the algorithm of asteroids number of each level here. it's like hard level.
    // 3 more asteroids each level.
    return (ASTEROID_INIT_NUMBER + (level_number-1)*3);
}

int _level_init(Level* level, int level_number)
{
    level->level_number = level_number;
    level->asteroid_total = _level_asteroids_number_calculator(level_number);
    level->game_status = NEW_LEVEL_NUMBER;  // should display LEVEL X when new get into a new level.
    level->count_down_of_game_status[NEW_LEVEL_NUMBER] = DEFAULT_LEVEL_COUNTDOWN;
    level->count_down_of_game_status[LEVEL_START] = DEFAULT_START_COUNTDOWN;
    level->count_down_of_game_status[IN_GAME_PLAY] = -1;
    level->count_down_of_game_status[LEVEL_WIN] = DEFAULT_WIN_COUNTDOWN;
    //DEFAULT_START_COUNTDOWN, -1, DEFAULT_WIN_COUNTDOWN];
//    level->display_level_number_countdown = DEFAULT_LEVEL_COUNTDOWN;    // countdown back to full, every countdown has to be counted down before go to next level.
//    level->display_start_countdown = DEFAULT_START_COUNTDOWN;
//    level->display_win_countdown = DEFAULT_WIN_COUNTDOWN;   //
    return 0;
}

/*
 The very first init, set to level 1, only called once.
 */
int level_first_init(Level* level)
{
    return _level_init(level, 1);
}

int level_go_next(Level* level)
{
    // init the level to the intial state, but with new level number and asteroids.
    return _level_init(level, level->level_number+1);
}

/*
 Tick, if current status has a countdown, tick it down once
 Return value will be value after tick down.
 */
float level_tick(Level* level)
{
    switch (level->game_status) {
        case IN_GAME_PLAY:      // fall through to GAME_OVER
        case NEXT_LEVEL:
        case GAME_OVER:
            return -1.0;  // These 3 won't tick.
            break;
        case LEVEL_WIN:
        case NEW_LEVEL_NUMBER:
        case LEVEL_START:
            if (level->count_down_of_game_status[level->game_status] > 0 ) {
                // count down not over yet.
                level->count_down_of_game_status[level->game_status] -= 1.0/GAME_FPS;
                return level->count_down_of_game_status[level->game_status];
            } else {
                // count down is over.
                level->count_down_of_game_status[level->game_status] = 0;
                // move to next stage.
                level->game_status++;
                return 0.0;
            }
            break;
        default:
            break;  // should do everything explicitly, default is dangrous to future changing.
    }
}





int level_destroy(Level* level)
{
    free(level);
    return 0;
}
