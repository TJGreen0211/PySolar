#ifndef QUADTREE_H
#define QUADTREE_H

#include <windows.h>
#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include <stdio.h>
#include "matrixMath.h"

typedef struct quadtree_node quadtree_node;
typedef struct quadtree quadtree;


struct quadtree_node {
    int key;
    mat4 position;
    vec3 center;
    float scale;
    quadtree_node *nw;
    quadtree_node *ne;
    quadtree_node *sw;
    quadtree_node *se;
};

struct quadtree {
    quadtree_node *root;
};


quadtree_node *quadtree_node_init();
void quadtree_node_free(quadtree_node *node);
void quadtree_node_subdivide(quadtree_node *node, int remaining_levels);
quadtree *quadtree_init();
void quadtree_free(quadtree *qt);
void quadtree_update(quadtree *qt);

#endif