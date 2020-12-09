#include "shader.h"

static char *shader_read(char *filename)
{
	FILE *fp;
	char *content = NULL;
	
	int count = 0;
	if(filename != NULL)
	{
		fp = fopen(filename, "rt");
		if(fp != NULL){
			fseek(fp, 0, SEEK_END);
			count = ftell(fp);
			rewind(fp);
			if (count > 0) {
            	content = (char *)malloc(sizeof(char) * (count+1));
            	count = fread(content,sizeof(char),count,fp);
            	content[count] = '\0';
        	}
        }
        fclose(fp);
	}
	return content;	
}


GLuint shader_load(char *path, int type)
{	
	GLuint shader = glCreateShader(type);
	
	char *shaderStr = shader_read(path);
	
	//printf("Compiling %d, %s shader\n", type, path);
	GLint length = strlen(shaderStr);
	glShaderSource(shader, 1, (const char **)&shaderStr, &length);
	glCompileShader(shader);
	
	//Check shader
	GLint success;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if(success == GL_FALSE) {
		char log[1024];
		glGetShaderInfoLog(shader, sizeof(log), 0, log);
		printf("Shader compile info: \n %s \n", log);
		glDeleteShader(shader);
		return 1;
	}

	free(shaderStr);
	return shader;
}


GLuint shader_create_program(char *vertex_path, char *fragment_path, char *tess_control_path, char *tess_evaluation_path, char *geometry_path) {
    GLuint program = glCreateProgram();
    if(vertex_path != NULL) {
		unsigned int vertex = shader_load(vertex_path, GL_VERTEX_SHADER);
        glAttachShader(program, vertex);
		glDeleteShader(vertex);
    }
    if(fragment_path != NULL) {
		unsigned int fragment = shader_load(fragment_path, GL_FRAGMENT_SHADER);
        glAttachShader(program, fragment);
		glDeleteShader(fragment);
    }
    if(tess_control_path != NULL) {
		unsigned int tess_control = shader_load(tess_control_path, GL_TESS_CONTROL_SHADER);
        glAttachShader(program, tess_control);
		glDeleteShader(tess_control);
    }
    if(tess_evaluation_path != NULL) {
		unsigned int tess_evaluation = shader_load(tess_evaluation_path, GL_TESS_EVALUATION_SHADER);
        glAttachShader(program, tess_evaluation);
		glDeleteShader(tess_evaluation);
    }
    if(geometry_path != NULL) {
		unsigned int geometry = shader_load(geometry_path, GL_GEOMETRY_SHADER);
        glAttachShader(program, geometry);
		glDeleteShader(geometry);
    }

    glLinkProgram(program);
    GLint programSuccess;
	glGetProgramiv(program, GL_LINK_STATUS, &programSuccess);
	if(programSuccess == GL_FALSE) {
		char log[1024];
		glGetProgramInfoLog(program, sizeof(log), 0, log);
		printf("shader link info: \n %s \n", log);
		glDeleteProgram(program);
		return 1;
	}

    //opengl.glDetachShader(self.program, shader)

    return program;
}