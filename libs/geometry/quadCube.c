#include "geometry.h"

void subdivide_face(geometry *qc, float reverse, int divisions, int order[3]) {
    vec3 start = {{1.0, 1.0, reverse}};
	float offset = 2.0/((float)divisions);

	for(int i = 0; i < divisions; i++) {
		start.v[0] = 1.0;
		for(int j = 0; j < divisions; j++) {
            float faces[4][3];
            faces[0][order[0]] = start.v[0];   	   	faces[0][order[1]] = start.v[1];          faces[0][order[2]] = start.v[2];
			faces[1][order[0]] = start.v[0]-offset; faces[1][order[1]] = start.v[1];          faces[1][order[2]] = start.v[2];
			faces[2][order[0]] = start.v[0]-offset; faces[2][order[1]] = start.v[1]-offset;   faces[2][order[2]] = start.v[2];
            faces[3][order[0]] = start.v[0];        faces[3][order[1]] = start.v[1]-offset;   faces[3][order[2]] = start.v[2];

			if(start.v[2] == 1.0) {
                memcpy(&qc->points[qc->vertex_number++], &faces[0], sizeof(faces[0]));
                memcpy(&qc->points[qc->vertex_number++], &faces[1], sizeof(faces[0]));
                memcpy(&qc->points[qc->vertex_number++], &faces[2], sizeof(faces[0]));

                memcpy(&qc->points[qc->vertex_number++], &faces[0], sizeof(faces[0]));
                memcpy(&qc->points[qc->vertex_number++], &faces[2], sizeof(faces[0]));
                memcpy(&qc->points[qc->vertex_number++], &faces[3], sizeof(faces[0]));
			}
			else {
                memcpy(&qc->points[qc->vertex_number++], &faces[0], sizeof(faces[0]));
                memcpy(&qc->points[qc->vertex_number++], &faces[3], sizeof(faces[0]));
                memcpy(&qc->points[qc->vertex_number++], &faces[2], sizeof(faces[0]));

                memcpy(&qc->points[qc->vertex_number++], &faces[0], sizeof(faces[0]));
                memcpy(&qc->points[qc->vertex_number++], &faces[2], sizeof(faces[0]));
                memcpy(&qc->points[qc->vertex_number++], &faces[1], sizeof(faces[0]));
			}

			start.v[0] = start.v[0] - offset;
		}
		start.v[1] -= offset;
	}
}

void generate_smooth_normals(geometry *qc) {
	vec3 *vna = malloc(qc->vertex_number*sizeof(vec3));
	vec3 vn;
	for(int i = 0; i < qc->vertex_number; i++) {
		vec3 tempvn = {{0.0, 0.0, 0.0}};
		vn = qc->points[i];
		for(int j = 0; j < qc->vertex_number; j++) {
			if(vn.v[0] == qc->points[j].v[0] && vn.v[1] == qc->points[j].v[1] && vn.v[2] == qc->points[j].v[2]) {
				tempvn = vec3PlusEqual(tempvn, qc->normals[j]);
			}
		}
		vna[i] = vec3Normalize(tempvn);
	}

	for(int i = 0; i < qc->vertex_number; i++) {
		qc->normals[i] = vna[i];
	}
	free(vna);
}

void generate_tangents(geometry *qc) {
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

DLL_EXPORT void geometry_quadcube_create(int divisions, geometry *quadcube) {
	/*
	a,b,c,d,e,f,g,h
			 (-1,1,1)e-------f(1,1,1)
				   / |	   / |
				 /	 |	 /	 |
	   (-1,1,-1)a-------b(1,1|,-1)
		   (-1,-|1,1)g--|----h(1,-1,1)
				|  /	|   /
				|/		| /
	  (-1,-1,-1)c-------d(1,-1,-1)
	*/

	quadcube->points = malloc(divisions*divisions*sizeof(vec3)*6*6);
	quadcube->normals = malloc(divisions*divisions*sizeof(vec3)*6*6);
	quadcube->tangents = malloc(divisions*divisions*sizeof(vec3)*6*6);
	quadcube->tex_coords = malloc(divisions*divisions*sizeof(vec2)*6*6);

    //int order[3] = {0, 1, 2};
    //int order[3] = {2, 0, 1};
    //int order[3] = {1, 2, 0};

	quadcube->vertex_number = 0;

    int order[3] = {0, 1, 2};
	subdivide_face(quadcube, 1.0, divisions, order);
	subdivide_face(quadcube, -1.0, divisions, order);
    order[0] =2; order[1] = 0; order[2] = 1;
    subdivide_face(quadcube, 1.0, divisions, order);
	subdivide_face(quadcube, -1.0, divisions, order);
    order[0] = 1; order[1] = 2; order[2] = 0;
    subdivide_face(quadcube, 1.0, divisions, order);
	subdivide_face(quadcube, -1.0, divisions, order);

	quadcube->point_size = quadcube->vertex_number*sizeof(vec3);
	quadcube->normal_size = quadcube->vertex_number*sizeof(vec3);
	quadcube->tangent_size = quadcube->vertex_number*sizeof(vec3);
	quadcube->tex_coord_size = quadcube->vertex_number*sizeof(vec2);

	for(int i = 0; i < quadcube->vertex_number; i++) {
		quadcube->tex_coords[i].v[0] = quadcube->points[i].v[order[1]]; 
		quadcube->tex_coords[i].v[1] = quadcube->points[i].v[order[0]];

		quadcube->points[i] = vec3Normalize(quadcube->points[i]);
	}

	for(int i = 0; i < quadcube->vertex_number; i+=3)
	{
		vec3 one, two;

		one.v[0] = quadcube->points[i+1].v[0] - quadcube->points[i].v[0];
		one.v[1] = quadcube->points[i+1].v[1] - quadcube->points[i].v[1];
		one.v[2] = quadcube->points[i+1].v[2] - quadcube->points[i].v[2];

		two.v[0] = quadcube->points[i+2].v[0] - quadcube->points[i+1].v[0];
		two.v[1] = quadcube->points[i+2].v[1] - quadcube->points[i+1].v[1];
		two.v[2] = quadcube->points[i+2].v[2] - quadcube->points[i+1].v[2];

		vec3 normal = vec3Normalize(crossProduct(one, two));

		quadcube->normals[i] = normal;
		quadcube->normals[i+1] = normal;
		quadcube->normals[i+2] = normal;

	}

	/*v_tex_coords = vec2(
		(atan(a_position.x, a_position.z) / (2.0*3.1415926)) + 0.5, 
		-a_position.y * 0.5 + 0.5
	);*/

	//generate_smooth_normals(quadcube);
	generate_tangents(quadcube);
}

DLL_EXPORT void geometry_quadcube_create_face(geometry *quadcube_face, int divisions, int order[3], int reverse) {
	quadcube_face->points = malloc(divisions*divisions*sizeof(vec3)*6);
	quadcube_face->normals = malloc(divisions*divisions*sizeof(vec3)*6);
	quadcube_face->tangents = malloc(divisions*divisions*sizeof(vec3)*6);
	quadcube_face->tex_coords = malloc(divisions*divisions*sizeof(vec2)*6);

	vec3 *temp_normalized_points = malloc(divisions*divisions*sizeof(vec3)*6);

	quadcube_face->vertex_number = 0;

	subdivide_face(quadcube_face, (float)reverse, divisions, order);
	quadcube_face->point_size = quadcube_face->vertex_number*sizeof(vec3);
	quadcube_face->normal_size = quadcube_face->vertex_number*sizeof(vec3);
	quadcube_face->tangent_size = quadcube_face->vertex_number*sizeof(vec3);
	quadcube_face->tex_coord_size = quadcube_face->vertex_number*sizeof(vec2);

	for(int i = 0; i < quadcube_face->vertex_number; i++) {
		//printf("x: %f, y: %f, z: %f\n", quadcube_face->points[i].v[0], quadcube_face->points[i].v[1], quadcube_face->points[i].v[2]);
		
		quadcube_face->tex_coords[i].v[0] = (quadcube_face->points[i].v[order[1]] + 1.0)/2.0; 
		quadcube_face->tex_coords[i].v[1] = (quadcube_face->points[i].v[order[0]] + 1.0)/2.0;

		temp_normalized_points[i] = vec3Normalize(quadcube_face->points[i]);

		//quadcube_face->points[i] = vec3Normalize(quadcube_face->points[i]);
	}

	for(int i = 0; i < quadcube_face->vertex_number; i+=3)
	{
		vec3 one, two;

		one.v[0] = temp_normalized_points[i+1].v[0] - temp_normalized_points[i].v[0];
		one.v[1] = temp_normalized_points[i+1].v[1] - temp_normalized_points[i].v[1];
		one.v[2] = temp_normalized_points[i+1].v[2] - temp_normalized_points[i].v[2];

		two.v[0] = temp_normalized_points[i+2].v[0] - temp_normalized_points[i+1].v[0];
		two.v[1] = temp_normalized_points[i+2].v[1] - temp_normalized_points[i+1].v[1];
		two.v[2] = temp_normalized_points[i+2].v[2] - temp_normalized_points[i+1].v[2];

		vec3 normal = vec3Normalize(crossProduct(one, two));

		quadcube_face->normals[i] = normal;
		quadcube_face->normals[i+1] = normal;
		quadcube_face->normals[i+2] = normal;
	}
	generate_smooth_normals(quadcube_face);

	generate_tangents(quadcube_face);
}

DLL_EXPORT void geometry_quadcube_dealloc(geometry *quadcube) {
    free(quadcube->points);
    quadcube->points = NULL;
    free(quadcube->normals);
    quadcube->normals = NULL;
	free(quadcube->tangents);
    quadcube->tangents = NULL;
	free(quadcube->tex_coords);
    quadcube->tex_coords = NULL;
}