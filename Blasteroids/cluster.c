//
//  cluster.c
//  Blasteroids
//
//  Created by linxucc on 2020/11/22.
//  Copyright © 2020 lin. All rights reserved.
//


#include "cluster.h"


/*
 Destroy one blast node and it's blast. Free the memory.
 
 Important: This function is supposed to be called only by blastcluster_remove_node(), or the linked list may become broken, and unpredictable errors/bugs could occur.
 
 This one directly frees the memory the parameter points to, don't do any further check.
 */
int _generic_cluster_node_destroy(GenericClusterNode* cn_to_destroy)
{
    // destroy the blast first.
    free(cn_to_destroy->real_instance);
    // destroy the node itself.
    free(cn_to_destroy);
    
    return 0;
}

int generic_cluster_init(GenerictCluster* generic_cluster)
{
    generic_cluster->first_node = NULL;
    generic_cluster->last_node = NULL;
    return 0;
}

int generic_cluster_destroy(GenerictCluster* generic_cluster)
{
    GenericClusterNode* iter_node = generic_cluster->first_node;
    while (iter_node) {
        GenericClusterNode* next_node = iter_node->next;
        _generic_cluster_node_destroy(iter_node);
        iter_node = next_node;
    }
    return 0;
}

GenericClusterNode* generic_cluster_add_node(GenerictCluster* gc, void* instance_to_add)
{
    // prepare a new node
    GenericClusterNode* cn;
    cn = malloc(sizeof(GenericClusterNode));
    // set it's real blast instance to caller provided blast. (it's caller's job to prepare the blast)
    cn->real_instance = instance_to_add;
    // set new node's prev to formerly last_node
    cn->prev = gc->last_node;
    // set new node's next to NULL, since it's the new last.
    cn->next = NULL;
    // bn's 3 attributes, all checked.
    
    // add this new node to the list.
    if (gc->last_node) {
        // last_node exists.
        
        // handle the old/previous last node
        // add some assertion here, it should be NULL before assign to new one, for tracking potential error.
        if (gc->last_node->next == NULL) {
            // previous last node now points to our new node.
            gc->last_node->next = cn;
        }
        else {
            go_error("When adding node, last node exists, but last_node->next is not NULL");
            
        }
        
        // handle the cluster
        // the new last node should be our new node.
        gc->last_node = cn;
        
    }
    else {
        // last node is not exist (NULL)
        // should assert first node is also NULL, so it's confirmed the 0 node situation. otherwise some code may be broken.
        if (gc->first_node == NULL) {
            // now our new node is the only first and the last node. add the links.
            gc->first_node = cn;
            gc->last_node = cn;
        }
        else {
            // go error, should never reach.
            go_error("When adding node, last node is NULL, but first node is not NULL");
        }
    }
    return cn;
}

/*
 Note:
    gc = generic cluster
    cn = cluster node.
 */
int generic_cluster_remove_node(GenerictCluster* gc, GenericClusterNode* cn_to_remove)
{
    if (cn_to_remove->next == NULL) {
        // last node, could be 1 node situaion or normal last node.
        if (cn_to_remove->prev == NULL) {
            // one node situation.
            // once removed the only node in the cluster, there will be no node left in it.
            // set cluster's first and last node to NULL, which means 0 node in list.
            gc->last_node = NULL;
            gc->first_node = NULL;
            
            // destroy the only single node itself.
            _generic_cluster_node_destroy(cn_to_remove);
        }
        else {
            // normal last node case.
            // should assert prev->next points to bn_to_remove
            if (cn_to_remove->prev->next!=cn_to_remove) {
                go_error("when remove node, prev->next is not pointing to node_to_remove");
            }
            
            // node themselves.
            // once last node is removed, the prev node of the last node become the new last, which has a NULL to its next, set it.
            cn_to_remove->prev->next = NULL;
            
            // node cluster.
            // last's prev is the new last.
            gc->last_node = cn_to_remove->prev;
            
            // destroy the node.
            _generic_cluster_node_destroy(cn_to_remove);
        }
    }
    else
    {
        // not last node, could be first node, or a normal node in between.
        // check if it's first node
        if (cn_to_remove->prev == NULL) {
            // normal first node situation
            
            // first's next (2nd) will become the new first, which has a NULL prev.
            // assert it's point to current node.
            if (cn_to_remove->next->prev != cn_to_remove) {
                // go error
            }
            cn_to_remove->next->prev = NULL;
            
            // update the cluster, first is the new first. (current node's next (2nd))
            gc->first_node = cn_to_remove->next;
            
            // destroy the node.
            _generic_cluster_node_destroy(cn_to_remove);
        }
        else {
            // not last, not first
            // no cluster update involved.
            
            // prev's next should be current's next (assert now it's current)
            cn_to_remove->prev->next = cn_to_remove->next;
            // next's prev should be current's prev (assert now it's current)
            cn_to_remove->next->prev = cn_to_remove->prev;
            
            // destroy the node
            _generic_cluster_node_destroy(cn_to_remove);
        }
    }
    return 0;
}
 

int generic_cluster_iterate_and_do(GenerictCluster* gc, int(*action_function)(void*), int(*action_result_handler)(GenerictCluster*, GenericClusterNode*, int ret_code))
{
    if (!gc || !action_function) {
        // go error.
    }
    GenericClusterNode* iter_node = gc->first_node;
    while (iter_node) {
        // perform action to this node.
        int ret = action_function( iter_node->real_instance);
        // handle return value if handler is provided.
        if (action_result_handler) {
            action_result_handler(gc, iter_node, ret);
        }
        // set iter_node to next node
        iter_node = iter_node->next;
    }
    return 0;
}
