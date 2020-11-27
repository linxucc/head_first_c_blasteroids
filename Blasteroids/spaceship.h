//
//  spaceship.h
//  Blasteroids
//
//  Created by linxucc on 2020/11/21.
//  Copyright © 2020 lin. All rights reserved.
//

#ifndef spaceship_h
#define spaceship_h

#include <stdio.h>
#include <math.h>
#include <allegro5/allegro5.h>
#include "common.h"

typedef struct {
    // position
    float x,y;
    // heading, 0-360, if ever 360, go to 1.
    float heading;
    // scale factor
    float scale;
    // spd
    float speed;
    // just dead, if true, it will be wild for 3 seconds.
    float invincible_time;  // 0 for no (normal), >=1 for yes (just dead), value for total
    // color of the spaceship
    ALLEGRO_COLOR color;
} Spaceship;

// 5s invincible time if got hit once.
#define SPACE_SHIP_INVINCIBLE_TIME 5

int spaceship_init(Spaceship *ship, float x, float y);

int spaceship_reset_to_center(Spaceship *ship);

int spaceship_draw(Spaceship *ship);

int spaceship_update(Spaceship *ship);

int spaceship_accelerate(Spaceship *ship);

int spaceship_decelerate(Spaceship *ship);

int spaceship_turn_left(Spaceship *ship);

int spaceship_turn_right(Spaceship *ship);

int spaceship_just_hit(Spaceship *ship);

int spaceship_destroy(Spaceship *ship);


#endif /* spaceship_h */

