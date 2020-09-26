#include "sphere.h"

static void triangle(vec3 a, vec3 b, vec3 c, sphere *s, int *index)
{
	vec3 one = {{b.v[0] - a.v[0], b.v[1] - a.v[1], b.v[2] - a.v[2]}};
	vec3 two = {{c.v[0] - b.v[0], c.v[1] - b.v[1], c.v[2] - b.v[2]}};

	vec3 normal = vec3Normalize(crossProduct(one, two));
	s->normals[index[0]] = normal; s->points[index[0]] = a; index[0]++;
	s->normals[index[0]] = normal; s->points[index[0]] = b; index[0]++;
	s->normals[index[0]] = normal; s->points[index[0]] = c; index[0]++;
}

static vec3 unitCircle(vec3 p)
{
	float length = p.v[0]*p.v[0] + p.v[1]*p.v[1] + p.v[2]*p.v[2];
	vec3 t = {{0.0, 0.0, 0.0}};
	if(length > 0)
	{
		t.v[0] = p.v[0] / sqrt(length);
		t.v[1] = p.v[1] / sqrt(length);
		t.v[2] = p.v[2] / sqrt(length);
	}
	return t;
}

static void divideTriangle(vec3 a, vec3 b, vec3 c, int count, sphere *s, int *index)
{
	if(count > 0) {
		vec3 v1 = unitCircle(vec3Add(a, b));
		vec3 v2 = unitCircle(vec3Add(a, c));
		vec3 v3 = unitCircle(vec3Add(b, c));
		divideTriangle(  a, v1, v2, count - 1, s, index);
        divideTriangle(  c, v2, v3, count - 1, s, index);
        divideTriangle(  b, v3, v1, count - 1, s, index);
        divideTriangle( v1, v3, v2, count - 1, s, index);
	}
	else {
		triangle(a, b, c, s, index);
	}
}

void tetrahedron(sphere *s, int num_divisions)
{
	s->divisions = num_divisions;

	int numTriangles = pow(4, s->divisions+1);
	int numVertices = 3 * numTriangles;

	int mallocVertSize = numVertices*sizeof(vec3);
	int mallocNormSize = numVertices*sizeof(vec3);

	s->points = malloc(mallocVertSize);
	s->normals = malloc(mallocNormSize);

	vec3 v[4] = {
		{{ 0.0, 0.0, 1.0 }},
		{{ 0.0, 0.942809, -0.333333}},
		{{ -0.816497, -0.471405, -0.333333 }},
		{{ 0.816497, -0.471405, -0.333333 }}
    };

	int index = 0;
    divideTriangle( v[0], v[1], v[2], s->divisions, s, &index);
    divideTriangle( v[3], v[2], v[1], s->divisions, s, &index);
    divideTriangle( v[0], v[3], v[1], s->divisions, s, &index);
    divideTriangle( v[0], v[2], v[3], s->divisions, s, &index);

    s->vertexNumber = numVertices;
    s->size = mallocVertSize;
    s->nsize = mallocNormSize;
}

void deallocSphere(sphere *s) {
	free(s->points);
	s->points = NULL;
	free(s->normals);
	s->normals = NULL;
}