#include "octree.h"

octree *octree_init() {
    octree *o = (octree *)malloc(sizeof(octree));

    return o;
}