#include "quadCube.h"

void subdivideFace(quadCube *qc, double reverse, int divisions, int order[3]) {
    vec3 start = {{1.0, 1.0, reverse}};
	double offset = 2.0/((double)divisions);

	for(int i = 0; i < divisions; i++) {
		start.v[0] = 1.0;
		for(int j = 0; j < divisions; j++) {
            double faces[4][3];
            faces[0][order[0]] = start.v[0];   	   faces[0][order[1]] = start.v[1];          faces[0][order[2]] = start.v[2];
			faces[1][order[0]] = start.v[0]-offset;   faces[1][order[1]] = start.v[1];          faces[1][order[2]] = start.v[2];
			faces[2][order[0]] = start.v[0]-offset;   faces[2][order[1]] = start.v[1]-offset;   faces[2][order[2]] = start.v[2];
            faces[3][order[0]] = start.v[0];          faces[3][order[1]] = start.v[1]-offset;   faces[3][order[2]] = start.v[2];

			if(start.v[2] == 1.0) {
                memcpy(&qc->points[qc->vertexNumber++], &faces[0], sizeof(faces[0]));
                memcpy(&qc->points[qc->vertexNumber++], &faces[1], sizeof(faces[0]));
                memcpy(&qc->points[qc->vertexNumber++], &faces[2], sizeof(faces[0]));

                memcpy(&qc->points[qc->vertexNumber++], &faces[0], sizeof(faces[0]));
                memcpy(&qc->points[qc->vertexNumber++], &faces[2], sizeof(faces[0]));
                memcpy(&qc->points[qc->vertexNumber++], &faces[3], sizeof(faces[0]));


			}
			else {
                memcpy(&qc->points[qc->vertexNumber++], &faces[0], sizeof(faces[0]));
                memcpy(&qc->points[qc->vertexNumber++], &faces[3], sizeof(faces[0]));
                memcpy(&qc->points[qc->vertexNumber++], &faces[2], sizeof(faces[0]));

                memcpy(&qc->points[qc->vertexNumber++], &faces[0], sizeof(faces[0]));
                memcpy(&qc->points[qc->vertexNumber++], &faces[2], sizeof(faces[0]));
                memcpy(&qc->points[qc->vertexNumber++], &faces[1], sizeof(faces[0]));
			}

			start.v[0] = start.v[0] - offset;
		}
		start.v[1] -= offset;
	}
}

void generateSmoothNormals(quadCube *qc) {
	vec3 *vna = malloc(qc->vertexNumber*sizeof(vec3));
	vec3 vn;
	for(int i = 0; i < qc->vertexNumber; i++) {
		vec3 tempvn = {{0.0, 0.0, 0.0}};
		vn = qc->points[i];
		for(int j = 0; j < qc->vertexNumber; j++) {
			if(vn.v[0] == qc->points[j].v[0] && vn.v[1] == qc->points[j].v[1] && vn.v[2] == qc->points[j].v[2]) {
				tempvn = vec3PlusEqual(tempvn, qc->normals[j]);
			}
		}
		vna[i] = vec3Normalize(tempvn);
	}

	for(int i = 0; i < qc->vertexNumber; i++) {
		qc->normals[i] = vna[i];
	}
	free(vna);
}

void createCube(int divisions, quadCube *newQuadCube) {
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

	newQuadCube->points = malloc(divisions*divisions*sizeof(vec3)*6*6);
	newQuadCube->normals = malloc(divisions*divisions*sizeof(vec3)*6*6);

    //int order[3] = {0, 1, 2};
    //int order[3] = {2, 0, 1};
    //int order[3] = {1, 2, 0};

	newQuadCube->vertexNumber = 0;

    int order[3] = {0, 1, 2};
	subdivideFace(newQuadCube, 1.0, divisions, order);
	subdivideFace(newQuadCube, -1.0, divisions, order);
    order[0] =2; order[1] = 0; order[2] = 1;
    subdivideFace(newQuadCube, 1.0, divisions, order);
	subdivideFace(newQuadCube, -1.0, divisions, order);
    order[0] = 1; order[1] = 2; order[2] = 0;
    subdivideFace(newQuadCube, 1.0, divisions, order);
	subdivideFace(newQuadCube, -1.0, divisions, order);

	newQuadCube->size = newQuadCube->vertexNumber*sizeof(vec3);
	newQuadCube->nsize = newQuadCube->vertexNumber*sizeof(vec3);

	for(int i = 0; i < newQuadCube->vertexNumber; i++) {
		newQuadCube->points[i] = vec3Normalize(newQuadCube->points[i]);
	}

	for(int i = 0; i < newQuadCube->vertexNumber; i+=3)
	{
		vec3 one, two;

		one.v[0] = newQuadCube->points[i+1].v[0] - newQuadCube->points[i].v[0];
		one.v[1] = newQuadCube->points[i+1].v[1] - newQuadCube->points[i].v[1];
		one.v[2] = newQuadCube->points[i+1].v[2] - newQuadCube->points[i].v[2];

		two.v[0] = newQuadCube->points[i+2].v[0] - newQuadCube->points[i+1].v[0];
		two.v[1] = newQuadCube->points[i+2].v[1] - newQuadCube->points[i+1].v[1];
		two.v[2] = newQuadCube->points[i+2].v[2] - newQuadCube->points[i+1].v[2];

		vec3 normal = vec3Normalize(crossProduct(one, two));

		newQuadCube->normals[i] = normal;
		newQuadCube->normals[i+1] = normal;
		newQuadCube->normals[i+2] = normal;
	}

	generateSmoothNormals(newQuadCube);
}

void deallocCube(quadCube *newQuadCube) {
    free(newQuadCube->points);
    newQuadCube->points = NULL;
    free(newQuadCube->normals);
    newQuadCube->points = NULL;
}