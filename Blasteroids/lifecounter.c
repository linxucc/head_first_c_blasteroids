//
//  lifecounter.c
//  Blasteroids
//
//  Created by linxucc on 2020/11/21.
//  Copyright © 2020 lin. All rights reserved.
//


#include "lifecounter.h"


/*
 Life Indicator
 
 Important:
    1. To keep it simple, we don't yet free the indicator spaceship's memory after life lost, so potential memory leak could happen.
    2. Life could only be decreased, not they other way around, or there will be segment fault. If there's a way that could get back life, the total count of spaceship may exceed the initial one, the array access will fail, which should cause a segment fault.
 */


/*
 Init a empty life counter to initial values, with configurable options.
 life_total: how many lives at beginning
 draw_begin_x, draw_begin_y: where to draw the first indicator (assume from left to right), the x,y of first one
 size_factor: sacle how much of original space ship, when it's used as a indicator.
 */
int lifecounter_init(LifeCounter* life_counter)
{
    life_counter->life_left = LIFE_COUNTER_INIT_LIVES;
    life_counter->original_x = LIFE_COUNTER_POSITION_LEFT_X;
    life_counter->original_y = LIFE_COUNTER_POSTIION_LEFT_Y;
    
    // prepare the template space ship, set it as the first (left most) indicator.
    life_counter->indicator_template = malloc(sizeof(Spaceship));
    spaceship_init(life_counter->indicator_template, life_counter->original_x, life_counter->original_y);     // set template spaceship at left most x,y
    life_counter->indicator_template->scale = LIFE_COUNTER_SCALE;     // override the default size factor of spaceship.
    
    // remember the padding between indicators, when draw it will be used to seperate indicators properly.
    life_counter->indicator_padding = LIFE_COUNTER_PADDING;
    return 0;
}

/*
 Draw this life counter as its setting goes.
 */
int lifecounter_draw(LifeCounter* life_counter)
{
    int i = 0;
    // draw each life indicator once at a time, then move template to next place.
    while ((life_counter->life_left > 0) && (i<life_counter->life_left)) {
        // move template right to 1 padding space, if it's not the first indicator.
        if(i>0) {
            life_counter->indicator_template->x += life_counter->indicator_padding * life_counter->indicator_template->scale;
        }
        // draw the template
        spaceship_draw(life_counter->indicator_template);
        // move to next
        i++;
    }
    // after all the draw, reset template to left-most place.
    life_counter->indicator_template->x = life_counter->original_x;
    // y is not changed for now, so don't include it.
    return 0;
}

/*
 Once got hit, indicator should change, life -= 1, if not > 0, game over.
 Return value: how many life left after this hit.
 
 Important Note: the function callers are expected to deal with the returned life value, if it's 0, game should over, make sure it's over. >=1, game should continue.
  
 */
int life_after_die_once(LifeCounter* life_counter)
{
    // 1, it will be 0; 0 will be zero.
    return life_counter->life_left > 0 ? --(life_counter->life_left) : 0;
}

int lifecounter_add_one_life(LifeCounter* life_counter)
{
    return ++(life_counter->life_left);
}

int lifecounter_destroy(LifeCounter* life_counter)
{
    free(life_counter->indicator_template);
    free(life_counter);
    return 0;
}
