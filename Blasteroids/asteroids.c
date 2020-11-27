//
//  asteroids.c
//  Blasteroids
//
//  Created by linxucc on 2020/11/22.
//  Copyright © 2020 lin. All rights reserved.
//


#include "asteroids.h"


int asteroid_draw(Asteroid* asteroid)
{
    // set transform
    ALLEGRO_TRANSFORM transform;
    al_identity_transform(&transform);
    // which comes first which comes late matters.
    al_scale_transform(&transform, asteroid->scale, asteroid->scale);
    al_rotate_transform(&transform, DEGTORAD(asteroid->twist));
    al_translate_transform(&transform, asteroid->x, asteroid->y);
    al_use_transform(&transform);
    
    // draw at origin.
    al_draw_line(-20, 20, -25, 5, asteroid->color, 2.0f);
    al_draw_line(-25, 5, -25, -10, asteroid->color, 2.0f);
    al_draw_line(-25, -10, -5, -10, asteroid->color, 2.0f);
    al_draw_line(-5, -10, -10, -20, asteroid->color, 2.0f);
    al_draw_line(-10, -20, 5, -20, asteroid->color, 2.0f);
    al_draw_line(5, -20, 20, -10, asteroid->color, 2.0f);
    al_draw_line(20, -10, 20, -5, asteroid->color, 2.0f);
    al_draw_line(20, -5, 0, 0, asteroid->color, 2.0f);
    al_draw_line(0, 0, 20, 10, asteroid->color, 2.0f);
    al_draw_line(20, 10, 10, 20, asteroid->color, 2.0f);
    al_draw_line(10, 20, 0, 15, asteroid->color, 2.0f);
    al_draw_line(0, 15, -20, 20, asteroid->color, 2.0f);
    
    return 0;
}

int asteroid_update(Asteroid* asteroid)
{
    // OFF screen, it should appear on the other side of screen.
    asteroid->x = asteroid->x < 0 ? (asteroid->x + BUFFER_WIDTH) : ( asteroid->x > BUFFER_WIDTH ? (asteroid->x - BUFFER_WIDTH) : asteroid->x );
    asteroid->y = asteroid->y < 0 ? (asteroid->y + BUFFER_HEIGHT) : ( asteroid->y > BUFFER_HEIGHT ? (asteroid->y - BUFFER_HEIGHT) :  asteroid->y );
    
    // rotate itself
    asteroid->twist += asteroid->rot_velocity;
    // if it's over 360, mod it
    asteroid->twist = fmod(asteroid->twist, 360);
    
    // move towards heading.
    asteroid->x += asteroid->speed * (1.0/GAME_FPS) * sin(DEGTORAD(asteroid->heading));
    asteroid->y -= asteroid->speed * (1.0/GAME_FPS) * cos(DEGTORAD(asteroid->heading));
    
    return 0;
}

Asteroid* asteroid_create(float x,
                          float y,
                          float heading,   // in degree, 0-360
                          float twist,    // in degree, 0-360
                          float speed,    // in px/second
                          float rot_velocity, // in degree, 0-360/second
                          float scale,
                          int life_left)
{
    Asteroid* new_asteroid = malloc(sizeof(Asteroid));
    new_asteroid->x = x;
    new_asteroid->y = y;
    new_asteroid->heading = heading <0 ? (heading+360.0) : fmod(heading, 360.0);
    new_asteroid->twist = twist <0 ? (twist+360.0) : fmod(twist, 360.0);
    new_asteroid->speed = speed;
    new_asteroid->rot_velocity = rot_velocity;
    new_asteroid->scale = scale;
    new_asteroid->life_left = life_left;
    new_asteroid->color = al_map_rgb(255, 255, 255);
    // it's callers responsibility to assign it to a node and cluster.
    new_asteroid->node_ref = NULL;
    return new_asteroid;
}

Asteroid* asteroid_create_random()
{
    float random_x = random_between_f(1.0, (float)BUFFER_WIDTH);
    float random_y = random_between_f(1.0, (float)BUFFER_HEIGHT);
    float random_heading = random_between_f(0.0, 360.0);    // heading
    float random_twist = random_between_f(0.0, 360.0);    // twist
    float random_speed = random_between_f(0.0, ASTEROID_MAX_SPEED);   // speed in degree
    float random_rotational_speed = random_between_f(0.0, ASTEROID_MAX_ROT_SPEED);  // rotational speed
    float random_scale = random_between_f(ASTEROID_DEFAULT_SCALE_FACTOR_MIN, ASTEROID_DEFAULT_SCALE_FACTOR_MAX); // scale factor
    int random_life = random_between(1, ASTEROID_BASE_LIFE);    // random life of asteroid.
    // new asteroid, with randoms, and 1 for scale, 0 for hit times.
    Asteroid* random_asteroid = asteroid_create(random_x, random_y, random_heading, random_twist, random_speed, random_rotational_speed, random_scale, random_life);
    return random_asteroid;
}

// cluster basic operations.

int asteroid_cluster_init(AsteroidCluster* ac)
{
    generic_cluster_init(ac);
    return 0;
}

int asteroid_cluster_destroy(AsteroidCluster* ac)
{
    generic_cluster_destroy(ac);
    return 0;
}


int asteroid_cluster_add_asteroid(AsteroidCluster* ac, Asteroid* asteroid)
{
    AsteroidClusterNode* new_node = generic_cluster_add_node(ac, asteroid);
    // ref back to the new node.
    if (new_node) {
        asteroid->node_ref = new_node;
    } else {
        go_error("Failed when adding new asteroid to asteroid cluster, newly created cluster node is NULL.");
    }
    return 0;
}



int asteroid_cluster_remove_asteroid_node(AsteroidCluster* ac, AsteroidClusterNode* node_to_remove)
{
    generic_cluster_remove_node(ac, node_to_remove);
    return 0;
}




// public operations

// random asteroid creation.
int asteroid_cluster_add_a_random_asteroid(AsteroidCluster* all_asteroids)
{
    // make a brand new random asteroid.
    Asteroid* random_asteroid = asteroid_create_random();
    // add it into cluster.
    asteroid_cluster_add_asteroid(all_asteroids, random_asteroid);
    return 0;
}

int asteroid_cluster_add_some_random_asteroids(AsteroidCluster* all_asteroids, int total_asteroids)
{
    while (total_asteroids>0) {
        asteroid_cluster_add_a_random_asteroid(all_asteroids);
        total_asteroids--;
    }
    return 0;
}



// when hit, split to two.
int asteroid_hit_and_split(AsteroidCluster* all_asteroids, Asteroid* asteroid)
{
    // once got hit, if not used all it's life, split; else, dispear.
    if (asteroid->life_left) {  // life_left is int, minimum == 1.
        // 1, Create a new asteroid, append it to its cluster.
        // change heading by -45 degree, hit time ++, scale half, others same,
        Asteroid* new_asteroid = asteroid_create(
                                                 asteroid->x,
                                                 asteroid->y,
                                                 asteroid->heading-ASTEROID_DEFAULT_SPLIT_DEGREE,
                                                 asteroid->twist,
                                                 asteroid->speed,
                                                 asteroid->rot_velocity,
                                                 asteroid->scale/ASTEROID_SCALE_FACTOR_WHEN_HIT,
                                                 asteroid->life_left -1
                                                 );
        // add it to all ateroids.
        asteroid_cluster_add_asteroid(all_asteroids, new_asteroid);
        // 2, change current asteroid's direction and scale, and hit times.
        asteroid->heading += ASTEROID_DEFAULT_SPLIT_DEGREE;
        asteroid->scale = asteroid->scale / ASTEROID_SCALE_FACTOR_WHEN_HIT;
        asteroid->life_left -= 1;
    }
    else {
        // destroy current asteroid.
        asteroid_cluster_remove_asteroid_node(all_asteroids, asteroid->node_ref);
        // Play VFX, added later.
    }
    return 0;
}


// update and draw all the asteroids, for surpress the warning of incompatible pointer type in function pointer parameters.
int asteroid_update_wrapper(void* asteroid)
{
    return asteroid_update((Asteroid*) asteroid);
}

int asteroid_draw_wrapper(void* asteroid)
{
    return asteroid_draw((Asteroid*) asteroid);
}

int asteroid_cluster_update(AsteroidCluster* ac)
{
    generic_cluster_iterate_and_do(ac, asteroid_update_wrapper, NULL);
    return 0;
}

int asteroid_cluster_draw(AsteroidCluster* ac)
{
    generic_cluster_iterate_and_do(ac, asteroid_draw_wrapper, NULL);
    return 0;
}

