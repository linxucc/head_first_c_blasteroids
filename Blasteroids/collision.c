//
//  collision.c
//  Blasteroids
//
//  Created by linxucc on 2020/11/23.
//  Copyright © 2020 lin. All rights reserved.
//

#include "collision.h"


/*
 Bounding box.
 
 v stands for vertex, 1,2 stands for left_top point and right bottom point.
 
 Different object may have different shape of box, that's implemented seperately in each object's individual bbox generate function.
 
 */
typedef struct {
    // four points of the box.
    float v1_x, v1_y;
    float v2_x, v2_y;
} _BBOX;

// Note: return _BBOX as value, so in function calls it will be passed by value-in-copy, so that there's no memory management problem.

_BBOX _BBOX_from_spaceship(Spaceship* ship)
{
    // a spaceship takes a space of -8 to +8 in x, -11 to 9 in y, relative to it's x,y
    // rotation is too complicated, use simple way instead.
    float offset = 11 * ship->scale;  // when scale, BBOX should apply this scale.
    _BBOX bbox = {
        .v1_x = ship->x-offset, .v1_y = ship->y-offset,  // (-8,-11)
        .v2_x = ship->x+offset, .v2_y = ship->y+offset,  // (+8, -11)
    };
    return bbox;
}

_BBOX _BBOX_from_asteroid(Asteroid* the_asteroid)
{
    // an asteroid takes up a space of -25 to 20 in x, -20 to 20 in y, relative to it's x,y
    // rotation... for anoter day...
    float offset = 25 * the_asteroid->scale;  // when scale, BBOX should apply this scale.
    _BBOX bbox = {
        .v1_x = the_asteroid->x-offset, .v1_y = the_asteroid->y-offset,  // (-25,-20)
        .v2_x = the_asteroid->x+offset, .v2_y = the_asteroid->y+offset,  // (+20, -20)
    };
    return bbox;
}

_BBOX _BBOX_from_blast(Blast* the_blast)
{
    // The bounding box of a blast is the rectangle of it. left-top point is v1, right-bottom point is v2.
    // v1 with both less x and y, v2 with both greater x and y.
    // 0 degree is defined as north, which is opposite to our screen cordinates's y, so -cos(heading) to get the other end of the blast.
    float v1_x = the_blast->x < (the_blast->x + sin(DEGTORAD(the_blast->heading))*BLAST_LENGTH) ? the_blast->x : (the_blast->x + sin(DEGTORAD(the_blast->heading))*BLAST_LENGTH);
    float v1_y = the_blast->y < (the_blast->y - cos(DEGTORAD(the_blast->heading))*BLAST_LENGTH) ? the_blast->y : (the_blast->y - cos(DEGTORAD(the_blast->heading))*BLAST_LENGTH);
    float v2_x = the_blast->x > (the_blast->x + sin(DEGTORAD(the_blast->heading))*BLAST_LENGTH) ? the_blast->x : (the_blast->x + sin(DEGTORAD(the_blast->heading))*BLAST_LENGTH);
    float v2_y = the_blast->y > (the_blast->y - cos(DEGTORAD(the_blast->heading))*BLAST_LENGTH) ? the_blast->y : (the_blast->y - cos(DEGTORAD(the_blast->heading))*BLAST_LENGTH);
    _BBOX bbox = { v1_x, v1_y, v2_x, v2_y };
    return bbox;
}

/*
 Detect if two bounding box is collided or not.
 
 If collides, return 1, else, return 0.
 
 */
bool _BBOX_collide_or_not(_BBOX bbox1, _BBOX bbox2)
{
    if (bbox1.v1_x > bbox2.v2_x)  return false;
    if (bbox1.v2_x < bbox2.v1_x)  return false;
    if (bbox1.v1_y > bbox2.v2_y)  return false;
    if (bbox1.v2_y < bbox2.v1_y)  return false;
    return true;
}

bool is_asteroid_hit_spaceship_yn(Spaceship* ship, Asteroid* asteroid)
{
    return _BBOX_collide_or_not(_BBOX_from_asteroid(asteroid), _BBOX_from_spaceship(ship));
}

bool is_blast_hit_asteroid_yn(Blast* blast, Asteroid* asteroid)
{
    return _BBOX_collide_or_not(_BBOX_from_asteroid(asteroid), _BBOX_from_blast(blast));
}

// 1 for crash, 0 for not crash.
int ship_crash_detection(AsteroidCluster* all_asteroids, Spaceship* ship)
{
    // If ship is in it's invicible time, it will not crash.
    if (ship->invincible_time) {
        return 0;
    }
    // Check if spaceship doesn't crash at every asteroids.
    AsteroidClusterNode* asteroid_iterator = all_asteroids->first_node;
    // loop till it's NULL
    while (asteroid_iterator) {
        if (is_asteroid_hit_spaceship_yn(ship, asteroid_iterator->real_instance)) {
            spaceship_just_hit(ship);
            /*
             main game logic should not implemented here, it should goes to main.
             */
            return 1; // for crash.
        }
        asteroid_iterator = asteroid_iterator->next;
    }
    return 0;
}

int asteroid_hit_detection(AsteroidCluster* all_asteroids, BlastCluster* all_blasts)
{
    /*
     Check for all blasts and all asteroids, if any of them collides, score a point.
     After checking, return total score earned in this round.
     */
    int score_increment = 0;
    bool hit_flag = false;
    AsteroidClusterNode* asteroid_iterator = all_asteroids->first_node;
    while (asteroid_iterator) {
        BlastClusterNode* blast_iterator = all_blasts->first_node;
        while (blast_iterator) {
            if (is_blast_hit_asteroid_yn(blast_iterator->real_instance, asteroid_iterator->real_instance)) {
                
                // 1. increase the score, by step.
                score_increment += SCORE_STEP;
                
                // 2. asteroid got hit and split, one asteroid could only get hit once, so go on to next one.
                // there's a chance it will be deleted, so before remove current node, save the next one's link.
                AsteroidClusterNode* new_asteroid_iterator = asteroid_iterator->next;   // NULL is fine, while check will handle it.
                asteroid_hit_and_split(all_asteroids, asteroid_iterator->real_instance);
                asteroid_iterator = new_asteroid_iterator;
                // set hit flag, so in the outer while, it will not move to next (here already moved)
                hit_flag = true;
                
                // 3. remove the blast just hit, this round will not continue, it's OK.
                blastcluster_remove_node(all_blasts, blast_iterator);   // link between side is implemented in the cluseter remove node function.
                blast_iterator = NULL; // this will automatically break the inner while.
            }
            else {
                blast_iterator = blast_iterator->next;
            }
        }
        // check if in the inner while there's a hit
        if (hit_flag) {
            // no need to move the cursor forward, already points to next element.
            // set flag back to false.
            hit_flag = !hit_flag;
        }
        else {
            // not hit, have to move the cursor forward.
            asteroid_iterator = asteroid_iterator->next; // NULL will be handled by while check.
        }
    }
    return score_increment; // the socre earned in this round will be returned.
}
