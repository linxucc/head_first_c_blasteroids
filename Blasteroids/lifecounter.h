//
//  lifecounter.h
//  Blasteroids
//
//  Created by linxucc on 2020/11/21.
//  Copyright © 2020 lin. All rights reserved.
//

#ifndef lifecounter_h
#define lifecounter_h

#include <stdio.h>
#include <stdlib.h>
#include "spaceship.h"

typedef struct {
    int life_left;
    Spaceship* indicator_template;    // the template spaceship for draw life indicator.
    float indicator_padding;    // the distance between 2 life indicator, in px between original spaceship's metrics, before scalling, this padding will be scalled.
    float original_x, original_y;   // the left-most life indicator's location.
} LifeCounter;

int lifecounter_init(LifeCounter* life_counter);

int lifecounter_draw(LifeCounter* life_counter);

int life_after_die_once(LifeCounter* life_counter);

int lifecounter_add_one_life(LifeCounter* life_counter);

int lifecounter_destroy(LifeCounter* life_counter);


#endif /* lifecounter_h */

