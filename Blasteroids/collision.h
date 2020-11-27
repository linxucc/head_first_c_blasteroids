//
//  collision.h
//  Blasteroids
//
//  Created by linxucc on 2020/11/23.
//  Copyright © 2020 lin. All rights reserved.
//

/*
 This file defines collsion detection functions.
 
 Most of game logic relates with one object collides with another, so this module is more coupled with other modules, so other modules could get a clean look.
 */

#ifndef collision_h
#define collision_h

#include <stdio.h>
#include "spaceship.h"
#include "asteroids.h"
#include "blast.h"
#include "lifecounter.h"
#include "score.h"

/*
 Ship hit by asteroids.
 
 If crash, return 1, else, return 0.
 */
int ship_crash_detection(AsteroidCluster* all_asteroids, Spaceship* ship);

/*
 Asteroids hit by blasts.
 
 If no hit, return 0, else, return the score.
 
 Asteroids and blasts that are hit will get destroyed.
 (optional, later) a explosion VFX will be added where the asteroid is hit.
 */
int asteroid_hit_detection(AsteroidCluster* all_asteroids, BlastCluster* all_blasts);

#endif /* collision_h */
