#include "quadtree.h"


static int quadtree_node_in_boundary(quadtree_node *n, quadtree_point p) {
    //printf("p.x: %f, c->center.x: %f, c->width: %f: %d\n", p.x, n->center.x, (n->width / 2.0), p.x <= n->center.x + (n->width / 2.0));
    //printf("p.x: %f, c->center.x: %f, c->width: %f: %d\n", p.x, n->center.x, (n->width / 2.0), p.x >= n->center.x - (n->width / 2.0));
    //printf("p.y: %f, c->center.y: %f, c->height: %f: %d\n", p.y, n->center.y, (n->height / 2.0), p.y <= n->center.y + (n->height / 2.0));
    //printf("p.y: %f, c->center.y: %f, c->height: %f: %d\n", p.y, n->center.y, (n->height / 2.0), p.y >= n->center.y - (n->height / 2.0));
    return (
        p.x <= n->center.x + (n->width / 2.0) &&
        p.x >= n->center.x - (n->width / 2.0) &&
        p.y <= n->center.y + (n->height / 2.0) &&
        p.y >= n->center.y - (n->height / 2.0));
}

static int quadtree_node_in_boundary2(quadtree_node *n, quadtree_point p) {
    return (
        p.x <= n->center.x*n->scale + n->scale*n->width &&
        p.x >= n->center.x*n->scale - n->scale*n->width &&
        p.y <= n->center.y*n->scale + n->scale*n->height &&
        p.y >= n->center.y*n->scale - n->scale*n->height);
}


static int quadtree_node_in_distance(quadtree_node *n, quadtree_point p) {
    float dist = sqrt(fabs((p.x - n->center.x)*(p.x - n->center.x) + 
        (p.y - n->center.y)*(p.x - n->center.x))
    );
    return (dist < n->width && dist < n->height);
}


int quadtree_node_is_leaf(quadtree_node *node) {
    if (!node) {
        return 0;
    }
    return !(node->ne && node->nw && node->sw && node->se);
}



quadtree_node *quadtree_node_init(float scale, float width, float height, float center_x, float center_y, int depth, int max) {
    quadtree_node *node = (quadtree_node *)malloc(sizeof(quadtree_node));
    if(node) {
        //printf("quadtree_node_init: %f, %f, %f, %f\n", width, height, center_x, center_y);

        node->width = width;
        node->height = height;
        node->center.x = center_x;
        node->center.y = center_y;
        node->current_depth = depth;
        node->max_depth = max;
        node->scale = scale;
        
        node->nw = NULL;
        node->ne = NULL;
        node->sw = NULL;
        node->se = NULL;
    }

    return node;
}

void quadtree_node_subdivide(quadtree_node *node) {
    //if(node && quadtree_node_is_leaf(node)) {
    float child_width = node->width / 2.0;
    float child_height = node->height / 2.0;
    

    //printf("ne: %f, %f, %f, %f\n", child_width, child_height, node->center.x - (child_width/2.0), node->center.y + (child_height/2.0));
    //printf("nw: %f, %f, %f, %f\n", child_width, child_height, node->center.x + (child_width/2.0), node->center.y + (child_height/2.0));
    //printf("se: %f, %f, %f, %f\n", child_width, child_height, node->center.x - (child_width/2.0), node->center.y - (child_height/2.0));
    //printf("sw: %f, %f, %f, %f\n", child_width, child_height, node->center.x + (child_width/2.0), node->center.y - (child_height/2.0));

    node->nw = quadtree_node_init(node->scale, child_width, child_height, node->center.x - (child_width/2.0), node->center.y + (child_height/2.0), node->current_depth, node->max_depth);
    node->ne = quadtree_node_init(node->scale, child_width, child_height, node->center.x + (child_width/2.0), node->center.y + (child_height/2.0), node->current_depth, node->max_depth);
    node->sw = quadtree_node_init(node->scale, child_width, child_height, node->center.x - (child_width/2.0), node->center.y - (child_height/2.0), node->current_depth, node->max_depth);
    node->se = quadtree_node_init(node->scale, child_width, child_height, node->center.x + (child_width/2.0), node->center.y - (child_height/2.0), node->current_depth, node->max_depth);
    
}

void quadtree_node_insert(quadtree_node *node, quadtree_point point) {
    node->current_depth++;

    if(!quadtree_node_in_boundary2(node, point)) {
        //printf("Quadtree point out of bounds\n");
        return;
    }
    if(node->current_depth >= node->max_depth) {
        //printf("Max depth %d reached\n", node->current_depth);
        return;
    }

    quadtree_node_subdivide(node);

    quadtree_node_insert(node->nw, point);
    quadtree_node_insert(node->ne, point);
    quadtree_node_insert(node->sw, point);
    quadtree_node_insert(node->se, point);
}

void quadtree_print_node(quadtree_node *node) {
    printf("Depth: %d, size: (%f, %f), center: (%f, %f)\n", node->current_depth, node->width, node->height, node->center.x, node->center.y);
}

void quadtree_search(quadtree_node *node) {
    if(quadtree_node_is_leaf(node)) {
        quadtree_print_node(node);
    } else {
        quadtree_search(node->nw);
        quadtree_search(node->ne);
        quadtree_search(node->sw);
        quadtree_search(node->se);
    }
}

quadtree_node *quadtree_create(float scale, int max_depth, quadtree_point point) {
    quadtree_node *node = (quadtree_node *)malloc(sizeof(quadtree_node));
    if(node) {
        node->width = 2.0;
        node->height = 2.0;
        node->max_depth = max_depth;
        node->current_depth = 0;
        node->scale = scale;
        node->center.x = 0.0;//width / 2.0;
        node->center.y = 0.0;//height / 2.0;
        node->nw = NULL;
        node->ne = NULL;
        node->sw = NULL;
        node->se = NULL;
        quadtree_node_insert(node, point);
    }
    return node;
}
