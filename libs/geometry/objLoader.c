#include "geometry.h"

static char *seperate_line(char **stringp, const char *delim) {
	if (*stringp == NULL) { 
		return NULL;
	}
	char *token_start = *stringp;
	*stringp = strpbrk(token_start, delim);
	if (*stringp) {
		**stringp = '\0';
		(*stringp)++;
	}
	return token_start;
}

DLL_EXPORT void geometry_load_object_file(const char *fname, geometry *mesh)
{
	printf("Loading: %s\n", fname);

	FILE *fp;
	char buffer[100];
    char *input = 0;
    size_t cur_len = 0;
    size_t cur_max = 0;

	fp = fopen(fname, "r");
	if(!fp) {
      	printf("Error while opening the file.\n");
      	exit(1);
   	}

	int face_array_size = 32*3;
	int vertex_array_size = 32*3;
	float *verts = malloc(vertex_array_size*sizeof(float));
	int *faces = malloc(face_array_size*sizeof(int));
	
	int face_count = 0;
	int vertex_count = 0;
	while (fgets(buffer, sizeof(buffer), fp)) {	
		int is_vertex = 0;
		int is_face = 0;
		char *token, *str, *tofree;
		tofree = str = strdup(buffer);
		while ((token = seperate_line(&str, " "))) {
			if(is_vertex) { 
				if(vertex_count > vertex_array_size) {
					vertex_array_size *= 2;
					verts = realloc(verts, vertex_array_size*sizeof(float));
				}
				verts[vertex_count++] = (float)atof(token);
			}
			if(is_face) {
				char *p = strchr(token, '/');
				if (!p) {}
				else{
					*p = 0;
				}
				if(face_count >= face_array_size) {
					face_array_size *= 2;
					faces = realloc(faces, face_array_size*sizeof(float));
				}
				faces[face_count++] = atoi(token);
			}
			if(strcmp(token, "v") == 0) {
				is_vertex = 1;
			}
			if(strcmp(token, "f") == 0) {
				is_face = 1;
			}
		};
		free(tofree);
		
		size_t buf_len = strlen(buffer);
        if (cur_len + buf_len + 1 > cur_max)
        {
			size_t new_len = cur_max * 2 + 1;
            if (buf_len + 1 > new_len)
                new_len = buf_len + 1;
            char *extra = realloc(input, new_len);
            if (extra == 0)
                break;
            input = extra;
			
            cur_max = new_len;
		}
		strcpy(input + cur_len, buffer);
        cur_len += buf_len;
    }
    free(input);
	fclose(fp);

	vec3 one, two;
	int Index = 0;
	int a, b, c;
	mesh->normals = malloc(face_count*sizeof(vec3));
	mesh->points = malloc(face_count*sizeof(vec3));
	mesh->tangents = 0;
	mesh->tex_coords = 0;

	for(int i = 0; i < face_count; i+=3)
	{
		a = (faces[i+0]-1)*3;
		b = (faces[i+1]-1)*3;
		c = (faces[i+2]-1)*3;
		
		
		one.v[0] = verts[b+0] - verts[a+0];
		one.v[1] = verts[b+1] - verts[a+1];
		one.v[2] = verts[b+2] - verts[a+2];
		//printf("%f, %f, %f\n", one.v[0], one.v[1], one.v[2]);

		two.v[0] = verts[c+0] - verts[b+0];
		two.v[1] = verts[c+1] - verts[b+1];
		two.v[2] = verts[c+2] - verts[b+2];

		vec3 normal = vec3Normalize(crossProduct(one, two));

		mesh->normals[Index] = normal; mesh->points[Index].v[0] = verts[a+0]; mesh->points[Index].v[1] = verts[a+1]; mesh->points[Index].v[2] = verts[a+2]; Index++;
		mesh->normals[Index] = normal; mesh->points[Index].v[0] = verts[b+0]; mesh->points[Index].v[1] = verts[b+1]; mesh->points[Index].v[2] = verts[b+2]; Index++;
		mesh->normals[Index] = normal; mesh->points[Index].v[0] = verts[c+0]; mesh->points[Index].v[1] = verts[c+1]; mesh->points[Index].v[2] = verts[c+2]; Index++;
		//printf("%f, %f, %f\n", normal.v[0], normal.v[1], normal.v[2]);
	}
	free(verts);
	free(faces);

	mesh->vertex_number = Index;
	mesh->point_size = Index*sizeof(vec3);
	mesh->normal_size = Index*sizeof(vec3);
	mesh->tangent_size = 0;
	mesh->tex_coord_size = 0;
}


DLL_EXPORT void geometry_load_object_file_depricated(const char *fname, geometry *mesh)
{	
	FILE *fp;
	fp = fopen(fname, "r");

	//printf("%s\n",fname );
	
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

	printf("\nDEPRICATED\n");
	//for(int i = 0; i < vertCount; i++) {
	//	printf("%f, %f, %f\n", verts[i].v[0], verts[i].v[1], verts[i].v[2]);
	//}
	
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
		//printf("%f, %f, %f\n", one.v[0], one.v[1], one.v[2]);
	
		two.v[0] = verts[c].v[0] - verts[b].v[0];
		two.v[1] = verts[c].v[1] - verts[b].v[1];
		two.v[2] = verts[c].v[2] - verts[b].v[2];

		vec3 normal = vec3Normalize(crossProduct(one, two));

		mesh->normals[Index] = normal; mesh->points[Index] = verts[a]; Index++;
		mesh->normals[Index] = normal; mesh->points[Index] = verts[b]; Index++;
		mesh->normals[Index] = normal; mesh->points[Index] = verts[c]; Index++;
		//printf("%f, %f, %f\n", normal.v[0], normal.v[1], normal.v[2]);
	}
	
	//for(int i = 0; i < Index; i++) {
	//	printf("%f, %f, %f\n", mesh->points[i].v[0], mesh->points[i].v[1], mesh->points[i].v[2]);
	//}
	
	free(verts);
	free(faces);

	mesh->vertex_number = Index;
	mesh->point_size = Index*sizeof(vec3);
	mesh->normal_size = Index*sizeof(vec3);
	mesh->tangent_size = 0;
	mesh->tex_coord_size = 0;

	//printf("Finished Loading File\n");
}

DLL_EXPORT void geometry_mesh_dealloc(geometry *mesh) {
    free(mesh->points);
    mesh->points = NULL;
    free(mesh->normals);
    mesh->normals = NULL;
	free(mesh->tangents);
    mesh->tangents = NULL;
	free(mesh->tex_coords);
    mesh->tex_coords = NULL;
}