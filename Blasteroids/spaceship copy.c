//
//  spaceship.c
//  Blasteroids
//
//  Created by 林旭骋 on 2020/11/21.
//  Copyright © 2020 lin. All rights reserved.
//

#ifndef spaceship_h
#include "spaceship.h"
#endif

/*
 Spaceship
 
 Key designs:
    1. Position:
        1.1 Initial position should be at the center of the screen.
        1.2 If it went out of the screen, it should appear on the other side.
        1.3 Position is calculated in float, cast to int on the fly when drawing.
    2. Speed and Acceleration:
        2.1 When the user is pressing UP, the speed of the spaceship increases; pressing DOWN, decreases.
        2.2 The space ship moves with its current speed, distance moved should be calculated with (1/fps)*spd.
        2.3 The acceleration per keypress, is a constant value. The max speed is also a constant value.
    3. Heading:
        3.1 heading is a float, between 0-1, 0 = 0 degree, 1 = 360 degree.
        3.2 The rate heading turns, is a constant number, it's X degree per keypress event.
    4. Live and Die:
        4.1 When space ship is hit, the total life decreases by 1, when life == 0, GAME OVER
        4.2 When space ship just lost a life, it will become invincible for a short period of time, during this time, any hit will be ignored, (the asteroid should be destroyed if hit by ship in this state), and the space ship should be flashing.
 
 Note:
    1. Space ship is also used to show the life indicator at left top, to save some code, and in case we change the design, the life will change all the way.
 */



/*
 Init the space ship.
 */
int spaceship_init(Spaceship *ship, float x, float y)
{
    ship->x = x;
    ship->y = y;
    ship->heading = 0.0;
    ship->scale = 1;
    ship->speed = 0.0;
    ship->just_dead = 0;
    ship->color = al_map_rgb(35, 122, 22); // the exact color from the book.
    return 0;
}

/*
 Draw the space ship, call allegro's draw directly.
 */
int spaceship_draw(Spaceship *ship)
{
    ALLEGRO_TRANSFORM transform;
    al_identity_transform(&transform);
    // which comes first which comes late matters.
    al_scale_transform(&transform, ship->scale, ship->scale);
    al_rotate_transform(&transform, DEGTORAD(ship->heading));
    al_translate_transform(&transform, ship->x, ship->y);
    al_use_transform(&transform);
    al_draw_line(-8, 9, 0, -11, ship->color, 3.0f);
    al_draw_line(0, -11, 8, 9, ship->color, 3.0f);
    al_draw_line(-6, 4, -1, 4, ship->color, 3.0f);
    al_draw_line(6, 4, 1, 4, ship->color, 3.0f);
    // should I dismiss the transform??? or may cause bug???
    return 0;
}

/*
 Update the space ship's data, to apply the accelerations.
 It's supposed to be called within every timer event, so the space ship could move accordingly between draws.
 */
int spaceship_update(Spaceship *ship)
{
    // move forward the ship, with the distance traveled from last draw (should be 1/FPS seconds).
    ship->x += ((ship->speed * (1.0/GAME_FPS)) * sin(DEGTORAD(ship->heading)));
    // screen coordinates's y is opposite from traditional ones in math.
    ship->y -= ((ship->speed * (1.0/GAME_FPS)) * cos(DEGTORAD(ship->heading)));
    
    // if it went off the screen, they should appear on the other side of screen.
    ship->x = (ship->x < 0) ? (fmod(ship->x, BUFFER_WIDTH) + BUFFER_WIDTH) : (fmod(ship->x, BUFFER_WIDTH));
    ship->y = (ship->y < 0) ? (fmod(ship->y, BUFFER_HEIGHT) + BUFFER_HEIGHT) : (fmod(ship->y, BUFFER_HEIGHT));

    
    return 0;
}

/*
 Increase the speed of the ship
 */
int spaceship_accelerate(Spaceship *ship)
{
    ship->speed = ship->speed >= MAX_SPEED ? ship->speed : (ship->speed + LINEAR_ACC);
    //ship->speed = (ship->speed + LINEAR_ACC) > MAX_SPEED ? MAX_SPEED : (ship->speed + LINEAR_ACC);
    return 0;
}

/*
 Decrease the speed of the ship
 */
int spaceship_decelerate(Spaceship *ship)
{
    ship->speed = (ship->speed - LINEAR_ACC) <= 0 ? 0.0 : (ship->speed - LINEAR_ACC);
    return 0;
}

/*
 Turn heading to left (conter-clockwise), when press LEFT
 */
int spaceship_turn_left(Spaceship *ship)
{
    ship->heading -= ANGLE_ACC;
    return 0;
}

/*
 Turn heading to right (clockwise), when press RIGHT
 */
int spaceship_turn_right(Spaceship *ship)
{
    ship->heading += ANGLE_ACC;
    return 0;
}
