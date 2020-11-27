//
//  blast.c
//  Blasteroids
//
//  Created by linxucc on 2020/11/21.
//  Copyright © 2020 lin. All rights reserved.
//

#include "common.h"
#include "blast.h"


int blast_init(Blast* b, float x, float y, float heading)
{
    // speed and color will be hardcoded for now.
    b->x = x;
    b->y = y;
    b->heading = heading;
    
    // by spec in the book, this should be 3 times the max speed of a space ship.
    b->speed = MAX_SPEED * 3;
    b->gone = 0; // 0 == not gone
    b->color = al_map_rgb(255, 255, 255);
    return 0;
}

/*
 Draw a blast
 
 A blast should be a dash with fixed length.
 */
int blast_draw(Blast* b)
{
    // draw code goes here.
    // How to draw:
    //      1. draw a 0 heading (north) line at first.
    //      2. transform this line to where it should be. (actual process is the opposite)
    
    // setup the transform.
    ALLEGRO_TRANSFORM transform;
    al_identity_transform(&transform);
    // which comes first which comes late matters.
    //al_scale_transform(&transform, ship->scale, ship->scale);
    al_rotate_transform(&transform, DEGTORAD(b->heading));
    al_translate_transform(&transform, b->x, b->y);
    al_use_transform(&transform);
    // from 0,0 to 0, -Blast_length,
    al_draw_line(0, 0, 0, 0-BLAST_LENGTH, b->color, BLAST_WIDTH);
    return 0;
}

/*
 Update a blast to the states of next moment
 
 Move a blast towards [heading] by [speed * delta-time]
 Speed is given, delta-time == 1/FPS
 
 Calculate new x and y, normally return 0;
 if x,y goes out of screen, set x,y to -10,-10 then return 1;
 
 Important: Caller should handle return 1 situation, and delete this blast from the cluster, then destroy both the node and the blast.
 */
int blast_update(Blast* b)
{
    // OFF screen situation,
    if (b->x > BUFFER_WIDTH || b->x < 0 || b->y > BUFFER_HEIGHT || b->y < 0) {
        // fail safe, move it out of scene first.
        b->x = -10; b->y = -10;
        // caller should handle the memory destroy things.
        return 1;
    }
    // Normal case, update one more step.
    b->x += b->speed * (1.0/GAME_FPS) * sin(DEGTORAD(b->heading));
    b->y -= b->speed * (1.0/GAME_FPS) * cos(DEGTORAD(b->heading));
    // 0 == success.
    return 0;
}



/*
 Destroy one blast node and it's blast. Free the memory.
 
 Important: This function is supposed to be called only by blastcluster_remove_node(), or the linked list may become broken, and unpredictable errors/bugs could occur.
 
 This one directly frees the memory the parameter points to, don't do any further check.
 */



int blastcluster_init(BlastCluster* blast_cluster)
{
    generic_cluster_init(blast_cluster);
    return 0;
}

int blastcluster_destroy(BlastCluster* blast_cluster)
{
    generic_cluster_destroy(blast_cluster);
    return 0;
}

int blastcluster_add_blast(BlastCluster* bc, Spaceship* ship)
{
    Blast* rb;  // rb = real blast
    rb = malloc(sizeof(Blast));
    blast_init(rb, ship->x, ship->y, ship->heading);
    
    generic_cluster_add_node(bc, rb);
    return 0;
}

int blastcluster_remove_node(BlastCluster* bc, BlastClusterNode* bn_to_remove)
{
    generic_cluster_remove_node(bc, bn_to_remove);
    return 0;
}


/*
 Iterate through all blasts, update each one's status.
 1. move every blast forward a little step;
 2. check if anyone exceeds the border of the screen, remove if any.
 
 Important: this assume for each frame draw, the COLLISION detection should be performed first, so that all the ones left, should not collide with other elements. So simply move all blasts forward a step will not cause contradictories. (a blast and a asteroid at the edge of the screen, collide always checks first) If after this update, should any of the blast collide with any asteroid, it's the next run's reposibility to check this collision and make the correct response. (blast hit a asteroid should be handled in the next run, nothing will miss).
 
 this run -> (collision dectection) -> (handle collision events) -> (every element move forward a step) -> next run, iterate this.
 
 */

/*
 local helper function, handle the off screen situaions
 */
int _blast_update_result_handler(GenerictCluster* gc, GenericClusterNode* cn, int ret_code)
{
    if (ret_code == 1)
        blastcluster_remove_node(gc, cn);
    return 0;
}
/*
 Pass blast_update directly to the iterator will cause a warning on function pointer's parameter type not match, to supress this, use this wrapper, to "cast" the function type to void, so that means, we are gonna past this Blast* function to somewhere expect it to be a void*
 */
int _blast_update_wrapper(void* blast)
{
    // explicitly cast the parameter type from void to Blast.
    return blast_update((Blast*)blast);
}

int blastcluster_update(BlastCluster* bc)
{
    // For each blast in this blustcluster,
    // DO blast_update()
    // Then handle the result (ret code) of each object, in _blast_update_result_handler()
    generic_cluster_iterate_and_do(bc, _blast_update_wrapper, _blast_update_result_handler);
    return 0;
}

/*
 Draw all the blast
 */
int _blast_draw_wrapper(void* blast)
{
    // explicitly cast the parameter type from void to Blast.
    return blast_draw((Blast*)blast);
}

int blastcluster_draw(BlastCluster* bc)
{
    generic_cluster_iterate_and_do(bc, _blast_draw_wrapper, NULL);
    return 0;
}
