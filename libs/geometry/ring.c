#include "geometry.h"

#define degToRad (M_PI / 180.0)

DLL_EXPORT void generate_ring_tangents(geometry *qc) {
	vec3 edge1, edge2, delta_uv1, delta_uv2;
	vec3 *bitangents = malloc(qc->vertex_number*sizeof(vec3));

	for(int i = 0; i < qc->vertex_number; i+=3) {
		edge1.v[0] = qc->points[i+1].v[0] - qc->points[i].v[0];
		edge1.v[1] = qc->points[i+1].v[1] - qc->points[i].v[1];
		edge1.v[2] = qc->points[i+1].v[2] - qc->points[i].v[2];
		edge2.v[0] = qc->points[i+2].v[0] - qc->points[i].v[0];
		edge2.v[1] = qc->points[i+2].v[1] - qc->points[i].v[1];
		edge2.v[2] = qc->points[i+2].v[2] - qc->points[i].v[2];

		delta_uv1.v[0] = qc->points[i+1].v[0] - qc->points[i].v[0];
		delta_uv1.v[1] = qc->points[i+1].v[1] - qc->points[i].v[1];
		delta_uv2.v[0] = qc->points[i+2].v[0] - qc->points[i].v[0];
		delta_uv2.v[1] = qc->points[i+2].v[1] - qc->points[i].v[1];

		float f = 1.0 / (delta_uv1.v[0] * delta_uv2.v[1] - delta_uv2.v[0] * delta_uv1.v[1]);
		qc->tangents[i].v[0] = f  * (delta_uv2.v[1] * edge1.v[0] - delta_uv1.v[1] * edge2.v[0]);
		qc->tangents[i].v[1] = f  * (delta_uv2.v[1] * edge1.v[1] - delta_uv1.v[1] * edge2.v[1]);
		qc->tangents[i].v[2] = f  * (delta_uv2.v[1] * edge1.v[2] - delta_uv1.v[1] * edge2.v[2]);
		qc->tangents[i] = vec3Normalize(qc->tangents[i]);
		for(int j = i; j < i+3; j++) {
			qc->tangents[j] = qc->tangents[i];
		}

		bitangents[i].v[0] = f * (delta_uv2.v[0] * edge1.v[0] - delta_uv1.v[0] * edge2.v[0]);
		bitangents[i].v[1] = f * (delta_uv2.v[0] * edge1.v[1] - delta_uv1.v[0] * edge2.v[1]);
		bitangents[i].v[2] = f * (delta_uv2.v[0] * edge1.v[2] - delta_uv1.v[0] * edge2.v[2]);
		bitangents[i] = vec3Normalize(bitangents[i]);

		for(int j = i; j < i+3; j++)
			bitangents[j] = bitangents[i];
	}

	for (int i = 0; i < qc->vertex_number; i+=3)
 	{
 		vec3 n = qc->normals[i];
 		vec3 t = qc->tangents[i];
		vec3 temp;

 		// Gram-Schmidt orthogonalize
		float d = vec3Dot(n, t);
		temp.v[0] = t.v[0] - n.v[0] * d;
		temp.v[1] = t.v[1] - n.v[1] * d;
		temp.v[2] = t.v[2] - n.v[2] * d;
 		qc->tangents[i] = vec3Normalize(temp);
		qc->tangents[i+1] = qc->tangents[i];
		qc->tangents[i+2] = qc->tangents[i];

 		// Calculate handedness
 		//tangent[a].w = (vec3Dot(crossProduct(n, t), bitangents[i]) < 0.0F) ? -1.0F : 1.0F;
 	}
	 free(bitangents);

}

void geometry_ring_create(int divisions, geometry *ring) {
    int index = 0;

    ring->vertex_number = 360*6;
    ring->point_size = ring->vertex_number*sizeof(vec3);
    ring->normal_size = ring->vertex_number*sizeof(vec3);
    ring->tangent_size = ring->vertex_number*sizeof(vec3);
    ring->tex_coord_size = ring->vertex_number*sizeof(vec2);

    ring->points = malloc(ring->point_size);
    ring->normals = malloc(ring->normal_size);
    ring->tangents = malloc(ring->tangent_size);
    ring->tex_coords = malloc(ring->tex_coord_size);

    vec3 one, two, normal;

    for(int i = 0; i < 360; i++) {
        float deg = i * degToRad;
        float outer_rad = 3.0;
        float inner_rad = 2.0;

        ring->points[index].v[0] = cos(deg)*outer_rad;
        ring->points[index].v[1] = sin(deg)*outer_rad;
        ring->points[index].v[2] = 0.0;
        ring->tex_coords[index].v[0] = 1.0;
        ring->tex_coords[index].v[1] = atan(ring->points[index].v[1]/ring->points[index].v[0]);

        ring->points[index+1].v[0] = cos(deg)*inner_rad;
        ring->points[index+1].v[1] = sin(deg)*inner_rad;
        ring->points[index+1].v[2] = 0.0;
        ring->tex_coords[index+1].v[0] = 0.0;
        ring->tex_coords[index+1].v[1] = atan(ring->points[index+1].v[1]/ring->points[index+1].v[0]);

        ring->points[index+2].v[0] = cos((i+1) * degToRad)*outer_rad;
        ring->points[index+2].v[1] = sin((i+1) * degToRad)*outer_rad;
        ring->points[index+2].v[2] = 0.0;
        ring->tex_coords[index+2].v[0] = 1.0;
        ring->tex_coords[index+2].v[1] = atan(ring->points[index+2].v[1]/ring->points[index+2].v[0]);

        one.v[0] = ring->points[index+1].v[0] - ring->points[index].v[0];
		one.v[1] = ring->points[index+1].v[1] - ring->points[index].v[1];
		one.v[2] = ring->points[index+1].v[2] - ring->points[index].v[2];

		two.v[0] = ring->points[index+2].v[0] - ring->points[index+1].v[0];
		two.v[1] = ring->points[index+2].v[1] - ring->points[index+1].v[1];
		two.v[2] = ring->points[index+2].v[2] - ring->points[index+1].v[2];

		normal = vec3Normalize(crossProduct(one, two));

        ring->normals[index] = normal;
		ring->normals[index+1] = normal;
		ring->normals[index+2] = normal;


        // Two
        ring->points[index+3].v[0] = cos((i+1) * degToRad)*outer_rad;
        ring->points[index+3].v[1] = sin((i+1) * degToRad)*outer_rad;
        ring->points[index+3].v[2] = 0.0;
        ring->tex_coords[index+3].v[0] = 1.0;
        ring->tex_coords[index+3].v[1] = atan(ring->points[index+3].v[1]/ring->points[index+3].v[0]);

        ring->points[index+4].v[0] = cos((i) * degToRad)*inner_rad;
        ring->points[index+4].v[1] = sin((i) * degToRad)*inner_rad;
        ring->points[index+4].v[2] = 0.0;
        ring->tex_coords[index+4].v[0] = 0.0;
        ring->tex_coords[index+4].v[1] = atan(ring->points[index+4].v[1]/ring->points[index+4].v[0]);

        ring->points[index+5].v[0] = cos((i+1) * degToRad)*inner_rad;
        ring->points[index+5].v[1] = sin((i+1) * degToRad)*inner_rad;
        ring->points[index+5].v[2] = 0.0;
        ring->tex_coords[index+5].v[0] = 0.0;
        ring->tex_coords[index+5].v[1] = atan(ring->points[index+5].v[1]/ring->points[index+5].v[0]);

        one.v[0] = ring->points[index+4].v[0] - ring->points[index+3].v[0];
		one.v[1] = ring->points[index+4].v[1] - ring->points[index+3].v[1];
		one.v[2] = ring->points[index+4].v[2] - ring->points[index+3].v[2];

		two.v[0] = ring->points[index+5].v[0] - ring->points[index+4].v[0];
		two.v[1] = ring->points[index+5].v[1] - ring->points[index+4].v[1];
		two.v[2] = ring->points[index+5].v[2] - ring->points[index+4].v[2];

		normal = vec3Normalize(crossProduct(one, two));

        ring->normals[index+3] = normal;
		ring->normals[index+4] = normal;
		ring->normals[index+5] = normal;

        index += 6;
    }
    generate_ring_tangents(ring);
}