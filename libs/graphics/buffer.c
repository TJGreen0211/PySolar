#include "buffer.h"

buffer buffer_init(geometry geom) {
	buffer buffer_object = {};
    glGenVertexArrays(1, &buffer_object.vao);
	glBindVertexArray(buffer_object.vao);
    glGenBuffers(1, &buffer_object.vbo);
    glBindBuffer(GL_ARRAY_BUFFER, buffer_object.vbo);

    glBufferData(GL_ARRAY_BUFFER, geom.point_size+geom.normal_size+geom.tangent_size+geom.tex_coord_size, NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, geom.point_size, geom.points);
	glBufferSubData(GL_ARRAY_BUFFER, geom.point_size, geom.normal_size, geom.normals);
	glBufferSubData(GL_ARRAY_BUFFER, geom.point_size+geom.normal_size, geom.tangent_size, geom.tangents);
	glBufferSubData(GL_ARRAY_BUFFER, geom.point_size+geom.normal_size+geom.tangent_size, geom.tex_coord_size, geom.tex_coords);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(GLfloat), BUFFER_OFFSET(0));
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3*sizeof(GLfloat), BUFFER_OFFSET(geom.point_size));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3*sizeof(GLfloat), BUFFER_OFFSET(geom.point_size+geom.normal_size));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 2*sizeof(GLfloat), BUFFER_OFFSET(geom.point_size+geom.normal_size+geom.tangent_size));
	glEnableVertexAttribArray(3);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

	buffer_object.vertex_number = geom.vertex_number;
	buffer_object.point_size = geom.point_size;
	buffer_object.normal_size = geom.normal_size;
	buffer_object.tangent_size = geom.tangent_size;
	buffer_object.tex_coord_size = geom.tex_coord_size;

    return buffer_object;
}