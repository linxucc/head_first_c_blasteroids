//
//  blast.c
//  Blasteroids
//
//  Created by 林旭骋 on 2020/11/21.
//  Copyright © 2020 lin. All rights reserved.
//

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
int _blastclusternode_destroy(BlastClusterNode* bn_to_destroy)
{
    // destroy the blast first.
    free(bn_to_destroy->real_blast);
    // destroy the node itself.
    free(bn_to_destroy);
    
    return 0;
}

int blastcluster_init(BlastCluster* blast_cluster)
{
    blast_cluster->first_node = NULL;
    blast_cluster->last_node = NULL;
    return 0;
}

int blastcluster_add_blast(BlastCluster* bc, Spaceship* ship)
{
    Blast* rb;
    rb = malloc(sizeof(Blast));
    blast_init(rb, ship->x, ship->y, ship->heading);
    // prepare a new node
    BlastClusterNode* bn;
    bn = malloc(sizeof(BlastClusterNode));
    // set it's real blast instance to caller provided blast. (it's caller's job to prepare the blast)
    bn->real_blast = rb;
    // set new node's prev to formerly last_node
    bn->prev = bc->last_node;
    // set new node's next to NULL, since it's the new last.
    bn->next = NULL;
    // bn's 3 attributes, all checked.
    
    // add this new node to the list.
    if (bc->last_node) {
        // last_node exists.
        
        // handle the old/previous last node
        // add some assertion here, it should be NULL before assign to new one, for tracking potential error.
        if (bc->last_node->next == NULL) {
            // previous last node now points to our new node.
            bc->last_node->next = bn;
        }
        else {
            // go error
        }
        
        // handle the cluster
        // the new last node should be our new node.
        bc->last_node = bn;
        
    }
    else {
        // last node is not exist (NULL)
        // should assert first node is also NULL, so it's confirmed the 0 node situation. otherwise some code may be broken.
        if (bc->first_node == NULL) {
            // now our new node is the only first and the last node. add the links.
            bc->first_node = bn;
            bc->last_node = bn;
        }
        else {
            // go error, should never reach.
        }
    }
    return 0;
}

int blastcluster_remove_node(BlastCluster* bc, BlastClusterNode* bn_to_remove)
{
    if (bn_to_remove->next == NULL) {
        // last node, could be 1 node situaion or normal last node.
        if (bn_to_remove->prev == NULL) {
            // one node situation.
            // once removed the only node in the cluster, there will be no node left in it.
            // set cluster's first and last node to NULL, which means 0 node in list.
            bc->last_node = NULL;
            bc->first_node = NULL;
            
            // destroy the only single node itself.
            _blastclusternode_destroy(bn_to_remove);
        }
        else {
            // normal last node case.
            // should assert prev->next points to bn_to_remove
            if (bn_to_remove->prev->next!=bn_to_remove) {
                // go error.
            }
            
            // node themselves.
            // once last node is removed, the prev node of the last node become the new last, which has a NULL to its next, set it.
            bn_to_remove->prev->next = NULL;
            
            // node cluster.
            // last's prev is the new last.
            bc->last_node = bn_to_remove->prev;
            
            // destroy the node.
            _blastclusternode_destroy(bn_to_remove);
        }
    }
    else
    {
        // not last node, could be first node, or a normal node in between.
        // check if it's first node
        if (bn_to_remove->prev == NULL) {
            // normal first node situation
            
            // first's next (2nd) will become the new first, which has a NULL prev.
            // assert it's point to current node.
            if (bn_to_remove->next->prev != bn_to_remove) {
                // go error
            }
            bn_to_remove->next->prev = NULL;
            
            // update the cluster, first is the new first. (current node's next (2nd))
            bc->first_node = bn_to_remove->next;
            
            // destroy the node.
            _blastclusternode_destroy(bn_to_remove);
        }
        else {
            // not last, not first
            // no cluster update involved.
            
            // prev's next should be current's next (assert now it's current)
            bn_to_remove->prev->next = bn_to_remove->next;
            // next's prev should be current's prev (assert now it's current)
            bn_to_remove->next->prev = bn_to_remove->prev;
            
            // destroy the node
            _blastclusternode_destroy(bn_to_remove);
        }
    }
    return 0;
}

/*
 Iterate through the cluster, and try to process each blast with the process function.
 
 Important: Should not be called directly.
 */

int _iterate_blastcluster_and_process_blast(int(*process_function)(Blast*), BlastCluster* bc)
{
    /*
     should check bc == NULL.
     */
    BlastClusterNode* iter_node = bc->first_node;
    while (iter_node) {
        // process this node.
        int ret = process_function(iter_node->real_blast);
        // handle return value
        // blast_update case.
        if (process_function == blast_update) {
            // ret == 1 , means the blast just went out of screen, just remove it. Free the memory of node and blast.
            if (ret == 1) {
                blastcluster_remove_node(bc, iter_node);
            }
        }
        
        // set iter_node to next node
        iter_node = iter_node->next;
    }
    return 0;
}

/*
 Iterate through all blasts, update each one's status.
 1. move every blast forward a little step;
 2. check if anyone exceeds the border of the screen, remove if any.
 
 Important: this assume for each frame draw, the COLLISION detection should be performed first, so that all the ones left, should not collide with other elements. So simply move all blasts forward a step will not cause contradictories. (a blast and a asteroid at the edge of the screen, collide always checks first) If after this update, should any of the blast collide with any asteroid, it's the next run's reposibility to check this collision and make the correct response. (blast hit a asteroid should be handled in the next run, nothing will miss).
 
 this run -> (collision dectection) -> (handle collision events) -> (every element move forward a step) -> next run, iterate this.
 
 */
int blastcluster_update(BlastCluster* bc)
{
    _iterate_blastcluster_and_process_blast(blast_update, bc);
    return 0;
}

/*
 Draw all the blast
 */
int blastcluster_draw(BlastCluster* bc)
{
    _iterate_blastcluster_and_process_blast(blast_draw, bc);
    return 0;
}
