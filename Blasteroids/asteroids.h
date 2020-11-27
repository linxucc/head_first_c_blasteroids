//
//  asteroids.h
//  Blasteroids
//
//  Created by linxucc on 2020/11/22.
//  Copyright © 2020 lin. All rights reserved.
//

#ifndef asteroids_h
#define asteroids_h

#include <stdio.h>
#include <allegro5/allegro_color.h>
#include <allegro5/allegro_primitives.h>
#include <math.h>
#include "cluster.h"
#include "spaceship.h"
#include "blast.h"
#include "common.h"

/*
 Cluster node of one asteroid.
 */
typedef GenericClusterNode AsteroidClusterNode;

/*
 Cluster of All the asteroids
 
 Using generic cluster.
 */

typedef GenerictCluster AsteroidCluster;

/*
 Individual asteroid.
 */

typedef struct {
    float x;
    float y;
    float heading;   // in degree, 0-360
    float twist;    // in degree, 0-360
    float speed;    // in px/second
    float rot_velocity; // in degree, 0-360/second
    float scale;
    int life_left;
    ALLEGRO_COLOR color;
    AsteroidClusterNode* node_ref; // holds the ref to the data-structural counterpart object. so each asteroid will have information about clusters, makes remove and add easier.
} Asteroid;

/*
 public functions
 */

/*
 init an empty asteroid cluster.
 */
int asteroid_cluster_init(AsteroidCluster* ac);

int asteroid_cluster_destroy(AsteroidCluster* ac);

/*
 add a random asteroid
 */
int asteroid_cluster_add_a_random_asteroid(AsteroidCluster* all_asteroids);

int asteroid_cluster_add_some_random_asteroids(AsteroidCluster* all_asteroids, int total_asteroids);

/*
 when an asteroid hit and split
 */
int asteroid_hit_and_split(AsteroidCluster* all_asteroids, Asteroid* asteroid);


/*
 update all asteroids in the cluster towards next moment.
 */
int asteroid_cluster_update(AsteroidCluster* ac);

/*
 draw all the asteroids in the cluster.
 */
int asteroid_cluster_draw(AsteroidCluster* ac);




#endif /* asteroids_h */
