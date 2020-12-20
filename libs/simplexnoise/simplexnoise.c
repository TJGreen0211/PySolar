#include "simplexnoise.h"

static unsigned int create_texture(int width, int height) {
    unsigned int texture_id;
    glGenTextures(1, &texture_id);
	glBindTexture(GL_TEXTURE_2D, texture_id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glBindTexture(GL_TEXTURE_2D, 0);

    return texture_id;
}


void simplexnoise_init(simplexnoise *snoise, int width, int height) {
    snoise->width = width;
    snoise->height = height;
    static int perm[256]= {151,160,137,91,90,15,
        131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,8,99,37,240,21,10,23,
        190, 6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,35,11,32,57,177,33,
        88,237,149,56,87,174,20,125,136,171,168, 68,175,74,165,71,134,139,48,27,166,
        77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,55,46,245,40,244,
        102,143,54, 65,25,63,161, 1,216,80,73,209,76,132,187,208, 89,18,169,200,196,
        135,130,116,188,159,86,164,100,109,198,173,186, 3,64,52,217,226,250,124,123,
        5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,189,28,42,
        223,183,170,213,119,248,152, 2,44,154,163, 70,221,153,101,155,167, 43,172,9,
        129,22,39,253, 19,98,108,110,79,113,224,232,178,185, 112,104,218,246,97,228,
        251,34,242,193,238,210,144,12,191,179,162,241, 81,51,145,235,249,14,239,107,
        49,192,214, 31,181,199,106,157,184, 84,204,176,115,121,50,45,127, 4,150,254,
        138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180
    };

    float vertices[] = {
		-1.0f, -1.0f, 1.0f,
        -1.0f,  1.0f, 1.0f,
         1.0f,  1.0f, 1.0f,
         1.0f,  1.0f, 1.0f,
         1.0f, -1.0f, 1.0f,
        -1.0f, -1.0f, 1.0f
	};

    snoise->vertex_number = (sizeof(vertices)/sizeof(vertices[0]));

    glGenVertexArrays(1, &snoise->vao);
	glBindVertexArray(snoise->vao);
	glGenBuffers(1, &snoise->vbo);
	glBindBuffer(GL_ARRAY_BUFFER, snoise->vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(GLfloat), BUFFER_OFFSET(0));
	glEnableVertexAttribArray(0);
	glBindVertexArray(0);

    snoise->shader = shader_create_program("../shaders/noise.vert",
        "../shaders/noise.frag",NULL,NULL,NULL);

    char *pixels;
	int grad3[16][3] = {{0,1,1},{0,1,-1},{0,-1,1},{0,-1,-1},
	                   {1,0,1},{1,0,-1},{-1,0,1},{-1,0,-1},
	                   {1,1,0},{1,-1,0},{-1,1,0},{-1,-1,0}, // 12 cube edges
	                   {1,0,-1},{-1,0,-1},{0,-1,1},{0,1,1}}; // 4 more to make 16

	glGenTextures(1, &snoise->perm_texture);
	glBindTexture(GL_TEXTURE_2D, snoise->perm_texture);

	pixels = (char*)malloc(256*256*4);
	for(int i = 0; i < 256; i++) {
		for(int j =0; j < 256; j++) {
			int offset = (i*256+j)*4;
			char value = perm[(j+perm[i]) & 0xFF];
			pixels[offset] = grad3[value & 0x0F][0] * 64 + 64;
			pixels[offset+1] = grad3[value & 0x0F][1] * 64 + 64;
			pixels[offset+2] = grad3[value & 0x0F][2] * 64 + 64;
			pixels[offset+3] = value;
		}
	}

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 256, 256, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
    glBindTexture(GL_TEXTURE_2D, 0);

    free(pixels);

    unsigned char simplex4[][4] = {{0,64,128,192},{0,64,192,128},{0,0,0,0},
  	    {0,128,192,64},{0,0,0,0},{0,0,0,0},{0,0,0,0},{64,128,192,0},
  	    {0,128,64,192},{0,0,0,0},{0,192,64,128},{0,192,128,64},
  	    {0,0,0,0},{0,0,0,0},{0,0,0,0},{64,192,128,0},
  	    {0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},
  	    {0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},
  	    {64,128,0,192},{0,0,0,0},{64,192,0,128},{0,0,0,0},
  	    {0,0,0,0},{0,0,0,0},{128,192,0,64},{128,192,64,0},
  	    {64,0,128,192},{64,0,192,128},{0,0,0,0},{0,0,0,0},
  	    {0,0,0,0},{128,0,192,64},{0,0,0,0},{128,64,192,0},
  	    {0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},
  	    {0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},
  	    {128,0,64,192},{0,0,0,0},{0,0,0,0},{0,0,0,0},
  	    {192,0,64,128},{192,0,128,64},{0,0,0,0},{192,64,128,0},
  	    {128,64,0,192},{0,0,0,0},{0,0,0,0},{0,0,0,0},
  	    {192,64,0,128},{0,0,0,0},{192,128,0,64},{192,128,64,0}
    };

  	glGenTextures(1, &snoise->simplex_texture);
  	glBindTexture(GL_TEXTURE_1D, snoise->simplex_texture);

  	glTexImage1D( GL_TEXTURE_1D, 0, GL_RGBA, 64, 0, GL_RGBA, GL_UNSIGNED_BYTE, simplex4 );
  	glTexParameteri( GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
  	glTexParameteri( GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
    glBindTexture(GL_TEXTURE_2D, 0);

    char *grad_pixels;
	int grad4[32][4]= {{0,1,1,1}, {0,1,1,-1}, {0,1,-1,1}, {0,1,-1,-1}, // 32 tesseract edges
	                   {0,-1,1,1}, {0,-1,1,-1}, {0,-1,-1,1}, {0,-1,-1,-1},
	                   {1,0,1,1}, {1,0,1,-1}, {1,0,-1,1}, {1,0,-1,-1},
	                   {-1,0,1,1}, {-1,0,1,-1}, {-1,0,-1,1}, {-1,0,-1,-1},
	                   {1,1,0,1}, {1,1,0,-1}, {1,-1,0,1}, {1,-1,0,-1},
	                   {-1,1,0,1}, {-1,1,0,-1}, {-1,-1,0,1}, {-1,-1,0,-1},
	                   {1,1,1,0}, {1,1,-1,0}, {1,-1,1,0}, {1,-1,-1,0},
	                   {-1,1,1,0}, {-1,1,-1,0}, {-1,-1,1,0}, {-1,-1,-1,0}};

	glGenTextures(1, &snoise->grad_texture);
	glBindTexture(GL_TEXTURE_2D, snoise->grad_texture);

	grad_pixels = (char*)malloc(256*256*4);
	for(int i = 0; i < 256; i++) {
		for(int j =0; j < 256; j++) {
			int offset = (i*256+j)*4;
			char value = perm[(j+perm[i]) & 0xFF];
			grad_pixels[offset] = grad4[value & 0x1F][0] * 64 + 64;
			grad_pixels[offset+1] = grad4[value & 0x1F][1] * 64 + 64;
			grad_pixels[offset+2] = grad4[value & 0x1F][2] * 64 + 64;
			grad_pixels[offset+3] = grad4[value & 0x1F][3] * 64 + 64;
		}
	}
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 256, 256, 0, GL_RGBA, GL_UNSIGNED_BYTE, grad_pixels);
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
    glBindTexture(GL_TEXTURE_2D, 0);

    free(grad_pixels);

    glGenFramebuffers(1, &snoise->noise_fboid);
    glBindFramebuffer(GL_FRAMEBUFFER, snoise->noise_fboid);
    snoise->render_texture = create_texture(snoise->width, snoise->height);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, snoise->render_texture, 0);

    unsigned int rbo;
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, snoise->width, snoise->height);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		printf("ERROR: Framebuffer is not complete");
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
}


void render_simplexnoise_texture(simplexnoise *snoise, float scale, float time, float seed_x, float seed_y, int order[3], int reverse) {
    //glViewport(0, 0, snoise->width, snoise->height);

    glBindFramebuffer(GL_FRAMEBUFFER, snoise->noise_fboid);
    //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);

    glUseProgram(snoise->shader);

    //u_loc = glGetUniformLocation(snoise->shader, b"projection");
    //glUniformMatrix4fv(u_loc, 1, False, np.array(perspective_arr).flatten().tobytes());
    //view_loc = glGetUniformLocation(snoise->shader, b"view");
    //glUniformMatrix4fv(view_loc, 1, False, view_arr.flatten().tobytes());
    //model_loc = glGetUniformLocation(snoise->shader, b"model");
    //glUniformMatrix4fv(model_loc, 1, False, np.identity(4, dtype=np.float32).flatten().tobytes());

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, snoise->perm_texture);
    glUniform1i(glGetUniformLocation(snoise->shader, "permTexture"), 1);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, snoise->simplex_texture);
    glUniform1i(glGetUniformLocation(snoise->shader, "simplexTexture"), 2);
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, snoise->grad_texture);
    glUniform1i(glGetUniformLocation(snoise->shader, "simplexTexture"), 3);
    glActiveTexture(GL_TEXTURE0);
    
    glUniform1f(glGetUniformLocation(snoise->shader, "systemTime"), time);
    glUniform1f(glGetUniformLocation(snoise->shader, "width"), snoise->width);
    glUniform1f(glGetUniformLocation(snoise->shader, "height"), snoise->height);
    glUniform1f(glGetUniformLocation(snoise->shader, "zoom"), 1.0);
    glUniform1f(glGetUniformLocation(snoise->shader, "terrain_scale"), scale);
    
    glUniform1i(glGetUniformLocation(snoise->shader, "animated"), 1);

    glUniform1i(glGetUniformLocation(snoise->shader, "reverse"), reverse);

    glUniform1i(glGetUniformLocation(snoise->shader, "order_x"), order[0]);
    glUniform1i(glGetUniformLocation(snoise->shader, "order_y"), order[1]);
    glUniform1i(glGetUniformLocation(snoise->shader, "order_z"), order[2]);

    char e_str[3];
    for(int i=0; i < 6; i++) {
        sprintf(e_str, "e%d", i);
        //printf("e: %s %f\n", e_str, snoise->e[i]);
        glUniform1i(glGetUniformLocation(snoise->shader, e_str), snoise->e[i]);
    }
    
    glBindVertexArray(snoise->vao);
    glDrawArrays(GL_TRIANGLES, 0, snoise->vertex_number);
    glBindVertexArray(0);
    
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    //GLubyte* pixels = (GLubyte*) malloc(snoise->width * snoise->height * sizeof(GLubyte) * 4);
    //glReadPixels(0, 0, snoise->width, snoise->height, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    
    //glActiveTexture(GL_TEXTURE1);
    //glBindTexture(GL_TEXTURE_2D, snoise->render_texture);
    //glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, snoise->width, snoise->height, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
    //glBindTexture(GL_TEXTURE_2D, 0);
    //glActiveTexture(GL_TEXTURE0);

    //free(pixels);
}