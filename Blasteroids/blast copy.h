//
//  blast.h
//  Blasteroids
//
//  Created by 林旭骋 on 2020/11/21.
//  Copyright © 2020 lin. All rights reserved.
//

#ifndef blast_h
#define blast_h

#include "common.h"
#include "spaceship.h"

/*
 Blast, purely for what a blast is.
 */
typedef struct {
    float x;
    float y;
    float heading;
    float speed;
    int gone; // 1 = gone, 0 = not gone.
    ALLEGRO_COLOR color;
} Blast;

/*
 Node of a double directional linked list to track all the blasts.
 
 1:1 mapping to Blast. Only handle linked list related operations.
 
 Blast itself should not contain certain data structrue information, that's not its job.
 
 last node: next == NULL;
 first node: prev == NULL;
 
 1 node situation: next == NULL and prev == NULL;
 0 node situation: no node at all. (Cluster should handle this.)
 
 */

typedef struct BlastClusterNode {
    Blast* real_blast;
    struct BlastClusterNode* prev;
    struct BlastClusterNode* next;
} BlastClusterNode;

/*
 Double directional linked list, store all the blasts.
 
 0 element situation: fn == NULL, ln == NULL
 1 element situation: fn == the same one itself, ln == the same one itself
 2 and above: fn == 1st, ln = 2nd or after.
 */
typedef struct {
    BlastClusterNode* first_node; // could be NULL, == no blast on screen.
    BlastClusterNode* last_node; // for quick insertion. could be NULL, == no blast on screen.
} BlastCluster;


/*
 Blastcluster - the double directional linked list to hold all the blast.
 
 This should be the public handler of all the blust related operation.
 
 Public functions:
    init(no param)
    new_blast(&spaceship) -- use the x,y,heading of the spaceship to create a blast, add it to the cluster.
    remove_blast(&BlastClusterNode) -- remove this node from cluster, and destroy node and blast.
    update_all_blasts(no param) -- iterate through all the nodes, move forward one step.
    draw_all_blasts(no param) -- iterate through all the nodes, draw each one of them.
 
 Note:
    Blast and BlastClusterNode should be private.
    All the blast related operations should be performed through BlastCluster.

 */

/*
 Return an empty blastcluster
 */
int blastcluster_init(BlastCluster* blast_cluster);

/*
 Add a new blast from current ship's location.
 */
int blastcluster_add_blast(BlastCluster* bc, Spaceship* ship);

/*
 Remove one blast
 */
int blastcluster_remove_node(BlastCluster* bc, BlastClusterNode* bn_to_remove);

int blastcluster_draw(BlastCluster* bc);

int blastcluster_update(BlastCluster* bc);


#endif /* blast_h */




