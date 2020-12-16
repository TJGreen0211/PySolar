#include "quadtree.h"

/*
static int node_in_boundary(quadtree_point p, quadtree t) {
    return (p.x <= t.top_left.x &&
        p.x >= t.bot_right.x &&
        p.y <= t.top_left.y &&
        p.y >= t.bot_right.y);
}*/

quadtree_node *quadtree_node_init() {
    quadtree_node *node = (quadtree_node *)malloc(sizeof(quadtree_node));

    return node;
}