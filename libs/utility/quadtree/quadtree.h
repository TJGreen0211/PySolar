#ifndef QUADTREE_H
#define QUADTREE_H

#include <stdlib.h>
#include <stdio.h>
#include "../matrixmath/matrixMath.h"

typedef struct quadtree_node quadtree_node;
typedef struct quadtree_point quadtree_point;
typedef struct quadtree quadtree;

struct quadtree_point {
    int x;
    int y;
};


struct quadtree_node {
    
    quadtree_point position;
    quadtree_node *ne;
    quadtree_node *nw;
    quadtree_node *se;
    quadtree_node *sw;

    //mat4 position;
    //vec3 center;
    //float scale;
    
};

struct quadtree {
    quadtree_node *root;

    quadtree_point top_left;
    quadtree_point bot_right;

    
};


void quadtree_insert(quadtree_node *node, quadtree tree);

#endif