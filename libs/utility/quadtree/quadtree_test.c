/*#include "quadtree.h"


static int node_in_boundary(quadtree_point p, quadtree t) {
    return (p.x <= t.top_left.x &&
        p.x >= t.bot_right.x &&
        p.y <= t.top_left.y &&
        p.y >= t.bot_right.y);
}

quadtree_cell *quadtree_cell_init(float center_x, float center_y, float distance_x, float distance_y) {
    quadtree_cell *cell = (quadtree_cell *)malloc(sizeof(quadtree_cell));
    if (cell) {
        cell->center.x = center_x;
        cell->center.y = center_y;
        cell->distance.x = distance_x;
        cell->distance.y = distance_y;
        cell->point = NULL;
    }
    return cell;

}

static int quadtree_node_is_leaf(quadtree_node *node) {
    if (!node) {
        return 0;
    }
    return !(node->ne && node->nw && node->sw && node->se);
}

static int quadtree_node_in_boundary(quadtree_cell *c, quadtree_point *p) {
    printf("p->x: %f, c->center.x + c->distance.x: %f: %d\n", p->x, c->center.x + c->distance.x, p->x <= c->center.x + c->distance.x);
    printf("p->x: %f, c->center.x - c->distance.x: %f: %d\n", p->x, c->center.x - c->distance.x, p->x >= c->center.x - c->distance.x);
    printf("p->y: %f, c->center.y + c->distance.y: %f: %d\n", p->y, c->center.y + c->distance.y, p->y <= c->center.y + c->distance.y);
    printf("p->y: %f, c->center.y - c->distance.y: %f: %d\n", p->y, c->center.y - c->distance.y, p->y >= c->center.y - c->distance.y);
    return (p->x <= c->center.x + c->distance.x &&
        p->x >= c->center.x - c->distance.x &&
        p->y <= c->center.y + c->distance.y &&
        p->y >= c->center.y - c->distance.y);
}

static int quadtree_cell_is_empty(quadtree_cell *c) {
    return !(c->point);
}

quadtree_node *quadtree_node_init(float center_x, float center_y, float distance_x, float distance_y) {
    quadtree_node *node = (quadtree_node *)malloc(sizeof(quadtree_node));
    if(node) {
        printf("quadtree_node_init: %f, %f, %f, %f\n", center_x, center_y, distance_x, distance_y);
        node->cell = quadtree_cell_init(center_x, center_x, distance_x, distance_y);
        node->nw = NULL;
        node->ne = NULL;
        node->sw = NULL;
        node->se = NULL;
    }

    return node;
}

void quadtree_node_subdivide(quadtree_node *node) {
    if(node && quadtree_node_is_leaf(node)) {
        float child_distance_x = node->cell->distance.x / 2.0;
        float child_distance_y = node->cell->distance.y / 2.0;

        node->ne = quadtree_node_init(node->cell->center.x + child_distance_x, node->cell->center.y + child_distance_y, child_distance_x, child_distance_y);
        node->nw = quadtree_node_init(node->cell->center.x - child_distance_x, node->cell->center.y + child_distance_y, child_distance_x, child_distance_y);
        node->sw = quadtree_node_init(node->cell->center.x - child_distance_x, node->cell->center.y - child_distance_y, child_distance_x, child_distance_y);
        node->se = quadtree_node_init(node->cell->center.x + child_distance_x, node->cell->center.y - child_distance_y, child_distance_x, child_distance_y);
    }
}

void quadtree_node_insert(quadtree_node *node, quadtree_point *point) {
    printf("quadtree_node_insert\n");
    if(node && quadtree_node_in_boundary(node->cell, point)) {
        printf("node && quadtree_node_in_boundary\n");
        if(quadtree_node_is_leaf(node)) {
            printf("quadtree_node_is_leaf\n");
            if(quadtree_cell_is_empty(node->cell)) {
                printf("quadtree_cell_is_empty\n");
                node->cell->point = point;
            } else {
                printf("Subdivide node\n");
                quadtree_node_subdivide(node);

                quadtree_node_insert(node->ne, node->cell->point);
                quadtree_node_insert(node->nw, node->cell->point);
                quadtree_node_insert(node->sw, node->cell->point);
                quadtree_node_insert(node->se, node->cell->point);

                quadtree_node_insert(node->ne, point);
                quadtree_node_insert(node->nw, point);
                quadtree_node_insert(node->sw, point);
                quadtree_node_insert(node->se, point);

                node->cell->point = NULL;
            }

        } else {
                quadtree_node_insert(node->ne, point);
                quadtree_node_insert(node->nw, point);
                quadtree_node_insert(node->sw, point);
                quadtree_node_insert(node->se, point);
        }
    }
}

quadtree *quadtree_init() {
    quadtree *qt = (quadtree *)malloc(sizeof(quadtree));
    if(qt) {
        qt->root = NULL;
    }
    return qt;
}
*/