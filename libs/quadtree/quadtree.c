#include "quadtree.h"

quadtree_node *quadtree_node_init() {
    quadtree_node *node = (quadtree_node*)malloc(sizeof(quadtree_node));
    if(node) {
        node->ne = NULL;
        node->nw = NULL;
        node->se = NULL;
        node->sw = NULL;
    }
    return node;
}

void quadtree_node_free(quadtree_node *node) {
    if(node) {
        quadtree_node_free(node->nw);
        quadtree_node_free(node->ne);
        quadtree_node_free(node->sw);
        quadtree_node_free(node->se);
        free(node);
    }
}

void quadtree_node_subdivide(quadtree_node *node, int remaining_levels) {
    if(node && remaining_levels > 0) {
        remaining_levels--;
        node->ne = quadtree_node_init();
        node->nw = quadtree_node_init();
        node->se = quadtree_node_init();
        node->sw = quadtree_node_init();

        quadtree_node_subdivide(node->ne, remaining_levels);
        quadtree_node_subdivide(node->nw, remaining_levels);
        quadtree_node_subdivide(node->se, remaining_levels);
        quadtree_node_subdivide(node->sw, remaining_levels);
    }
}

quadtree *quadtree_init() {
    quadtree *qt = (quadtree*)malloc(sizeof(quadtree));
    if(qt) {
        qt->root = NULL;
    }
    //
    //    qt->root = quadtree_node_init();
    //    quadtree_node_subdivide(qt->root, 3);
    //}
    return qt;
}

void quadtree_free(quadtree *qt) {
    if(qt) {
        quadtree_node_free(qt->root);
        free(qt);
    }
}

void quadtree_update(quadtree *qt) {
    quadtree_node_free(qt->root);
    qt->root = quadtree_node_init();
    quadtree_node_subdivide(qt->root, 3);
}

//quadtree_node *quadtree_node_insert(int key, quadtree_node **leaf) {
//    
//}
//
//quadtree_node *quadtree_node_search(int key, quadtree_node *leaf) {
//
//}