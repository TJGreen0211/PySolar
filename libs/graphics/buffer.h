#ifndef BUFFER_H
#define BUFFER_H

#include <stdlib.h>
#include "../glad/glad.h"
#include "../GLFW/glfw3.h"
#include "../geometry/geometry.h"
#include "../matrixmath/matrixMath.h"
#include <stdio.h>

typedef struct buffer {
    unsigned int vao;
    unsigned int vbo;
    int vertex_number;
    int point_size;
    int normal_size;
    int tangent_size;
    int tex_coord_size;
} buffer;

#define BUFFER_OFFSET(offset) ((char*)NULL+(offset))

buffer buffer_init(geometry geom);

#endif