//
//  cluster.h
//  Blasteroids
//
//  Created by linxucc on 2020/11/22.
//  Copyright © 2020 lin. All rights reserved.
//

/*
 Cluster is a double directional linked list, used for the storage of blasts and asteroids.
 
 The cluster itself should not contain any kind of
 */

#ifndef cluster_h
#define cluster_h

#include <stdio.h>
#include <stdlib.h>
#include "common.h"

/*
 Node of a double directional linked list to track all the blasts.
 
 1:1 mapping to Blast. Only handle linked list related operations.
 
 Blast itself should not contain certain data structrue information, that's not its job.
 
 last node: next == NULL;
 first node: prev == NULL;
 
 1 node situation: next == NULL and prev == NULL;
 0 node situation: no node at all. (Cluster should handle this.)
 
 */

typedef struct GenericClusterNode {
    void* real_instance;
    struct GenericClusterNode* prev;
    struct GenericClusterNode* next;
} GenericClusterNode;

/*
 Double directional linked list, store all the blasts.
 
 0 element situation: fn == NULL, ln == NULL
 1 element situation: fn == the same one itself, ln == the same one itself
 2 and above: fn == 1st, ln = 2nd or after.
 */
typedef struct {
    GenericClusterNode* first_node; // could be NULL, == no blast on screen.
    GenericClusterNode* last_node; // for quick insertion. could be NULL, == no blast on screen.
} GenerictCluster;


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
 Init an empty cluster.
 */
int generic_cluster_init(GenerictCluster* gc);

int generic_cluster_destroy(GenerictCluster* generic_cluster);

/*
 Add a new element (of any kind) to this cluster.
 
 Param:
    void* instance_to_add : a pointer to any kind of data to add in this cluster, this function will create a new node and set the target of this node to this pointer.
 */
GenericClusterNode* generic_cluster_add_node(GenerictCluster* gc, void* instance_to_add);

/*
 Remove one node
 */
int generic_cluster_remove_node(GenerictCluster* gc, GenericClusterNode* cn_to_remove);

/*
 Iterate through cluster, and perform provided action to all it's nodes.
 
 Param:
        int(*action_function)(void*): An action function, takes a pointer to the real object that each cluster node points to.
    int(*action_result_handler)(GenerictCluster*, void*, int))
    
 Note: Future should add a condition function, so it could perform 'if node meet such condition, do soemthing'.
 
 */
int generic_cluster_iterate_and_do(GenerictCluster* gc, int(*action_function)(void*), int(*action_result_handler)(GenerictCluster*, GenericClusterNode*, int ret_code));

GenericClusterNode* generic_cluster_get_first_node(GenerictCluster* gc);

GenericClusterNode* generic_cluster_node_get_next(GenericClusterNode* gn);


#endif /* cluster_h */
