#include "objLoader.h"

void load_object_file(char *fname, obj *mesh)
{
	FILE *fp;
	fp = fopen(fname, "r");
	
	char ch;
	int read;
	float x, y, z;
	int ix, iy, iz;
	int vertCount = 0;
   	int faceCount = 0;
   	int objectCount = 0;
   	int i = 0;
	int j = 0;

	
	if(!fp)
   	{
      	printf("Error while opening the file.\n");
      	exit(1);
   	}

   	
	while((ch = fgetc(fp)) != EOF)
	{
		read = fscanf(fp, "%c",&ch);
		if(read == 1 && strncmp(&ch, "o ", 1) == 0)
		{
			objectCount+=1;
			//printf("OBJECT COUNT: %d\n", objectCount);
		}
		if(read == 1 && ch == 'v')
		{
			fscanf(fp, "%f", &x);
			fscanf(fp, "%f", &y);
			fscanf(fp, "%f", &z);
			vertCount++; 
		}
		if(read == 1 && ch == 'f')
		{
			fscanf(fp, "%d", &ix);
			fscanf(fp, "%d", &iy);
			fscanf(fp, "%d", &iz);
			faceCount++;
		}
	}
	
	rewind(fp);
	
	vec3 *verts = malloc(vertCount*sizeof(vec3));
	vec3 *faces = malloc(faceCount*sizeof(vec3));
	int breakCount= 0;
	
	while((ch = fgetc(fp)) != EOF)
	{
		read = fscanf(fp, "%c",&ch);
		if(read == 1 && ch == 'o' && breakCount == 1)
		{
			breakCount = 1;
			
		}
		if(read == 1 && ch == 'v')
		{
			fscanf(fp, "%f", &x);
			fscanf(fp, "%f", &y);
			fscanf(fp, "%f", &z);
			verts[i].v[0] = x;
			verts[i].v[1] = y;
			verts[i].v[2] = z;
			//printf("%f, %f, %f\n", verts[i].x, verts[i].y, verts[i].z);
			i++;
		}
		if(read == 1 && ch == 'f')
		{
			fscanf(fp, "%d", &ix);
			fscanf(fp, "%d", &iy);
			fscanf(fp, "%d", &iz);
			faces[j].v[0] = ix;
			faces[j].v[1] = iy;
			faces[j].v[2] = iz;
			//printf("%f, %f, %f\n", faces[j].v[0], faces[j].v[1], faces[j].v[2]);
			j++;
		}
	}
	
	fclose(fp);

	printf("HERE\n");
	
	//vec3 normals[vertCount*3];
	//vec3 points[vertCount*3];
	vec3 one, two;
	int Index = 0;
	int a, b, c;
	
	mesh->normals = malloc(faceCount*3*sizeof(vec3));
	mesh->points = malloc(faceCount*3*sizeof(vec3));
	
	for(int i = 0; i < faceCount; i++)
	{
		a = faces[i].v[0]-1;
		b = faces[i].v[1]-1;
		c = faces[i].v[2]-1;
		
		one.v[0] = verts[b].v[0] - verts[a].v[0];
		one.v[1] = verts[b].v[1] - verts[a].v[1];
		one.v[2] = verts[b].v[2] - verts[a].v[2];
	
		two.v[0] = verts[c].v[0] - verts[b].v[0];
		two.v[1] = verts[c].v[1] - verts[b].v[1];
		two.v[2] = verts[c].v[2] - verts[b].v[2];

		vec3 normal = vec3Normalize(crossProduct(one, two));

		mesh->normals[Index] = normal; mesh->points[Index] = verts[a]; Index++;
		mesh->normals[Index] = normal; mesh->points[Index] = verts[b]; Index++;
		mesh->normals[Index] = normal; mesh->points[Index] = verts[c]; Index++;
	}
	
	
	free(verts);
	free(faces);
	
	mesh->size = Index*sizeof(vec3);
	mesh->nsize = Index*sizeof(vec3);
	mesh->vertexNumber = Index;

	printf("Finished Loading File\n");
}