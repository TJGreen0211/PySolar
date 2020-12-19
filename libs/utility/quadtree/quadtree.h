#ifndef QUADTREE_H
#define QUADTREE_H

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

typedef struct quadtree_cell quadtree_cell;
typedef struct quadtree_node quadtree_node;
typedef struct quadtree_point quadtree_point;
typedef struct quadtree quadtree;

struct quadtree_point {
    float x;
    float y;
};

struct quadtree_cell {
    quadtree_point center;
    quadtree_point distance;
    quadtree_point *point;
};


struct quadtree_node {
    int max_depth;
    int current_depth;
    float width;
    float height;
    float scale;

    quadtree_point center;

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
};

int quadtree_node_is_leaf(quadtree_node *node);
void quadtree_print_node(quadtree_node *node);
quadtree_node *quadtree_create(float scale, int max_depth, quadtree_point point);
void quadtree_node_insert(quadtree_node *node, quadtree_point point);
void quadtree_search(quadtree_node *node);

#endif