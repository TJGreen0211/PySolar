#include "main.h"

static float delta_time = 0.0;
static int actionPress, keys;
static arcball_camera camera;
static skybox space_skybox;


void reload_shader(starsystem *sol) {
    //glDeleteProgram(sol->wave_shader);
    //sol->wave_shader = shader_create_program("../shaders/planet2.vert",
    //    "../shaders/planet2.frag",NULL,NULL,NULL);
    glDeleteProgram(sol->planet_shader);
    sol->planet_shader = shader_create_program(
        "../shaders/planet.vert",
        "../shaders/planet.frag",
        "../shaders/planet.tcsh",
        "../shaders/planet.tesh",
        NULL);

    for(int i = 0; i < 6; i++) {
    glDeleteProgram(sol->planets[0].snoise_face[i].shader);
    sol->planets[0].snoise_face[i].shader = shader_create_program("../shaders/noise.vert",
        "../shaders/noise.frag",NULL,NULL,NULL);
    }
    int order_array[6][3] = {
        {0, 1, 2},
        {0, 1, 2},
        {2, 0, 1},
        {2, 0, 1},
        {1, 2, 0},
        {1, 2, 0}
    };
    int flip = 1;

    for(int i = 0; i < 6; i++) {
        render_simplexnoise_texture(&sol->planets[0].snoise_face[i], 1.1, 0.0, i*512.0, i*512.0, order_array[i], flip);
        //render_simplexnoise_texture(&s->planets[0].snoise_biomes[i], 0.0, i*s->snoise.width, i*s->snoise.height, order_array[i], flip);
        //s->planets[0].snoise_textures[i] = s->snoise.render_texture;
        flip *= -1;
    }


    //sol->planet_shader = shader_create_program("../shaders/planet2.vert",
    //    "../shaders/planet2.frag",NULL,NULL,NULL);

    //for(int i = 0; i < 6; i++) {
    //    glDeleteProgram(sol->planets[0].snoise_face[i].shader);
    //    sol->planets[0].snoise_face[i].shader = shader_create_program("../shaders/noise.vert",
    //        "../shaders/noise.frag",NULL,NULL,NULL);
    //}
    //skybox_image_shader = shader_create_program("../shaders/skybox_image.vert",
    //    "../shaders/skybox_image.frag",NULL,NULL,NULL);
    //space_skybox = skybox_init(2048);
}


void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
	if(action == GLFW_PRESS) {
    	actionPress = GLFW_PRESS;
    } else if(action == GLFW_RELEASE) {
    	actionPress = 0;
    }

    if (key == GLFW_KEY_R && action == GLFW_PRESS){
    	keys = GLFW_KEY_R;
    }

    if (key == GLFW_KEY_W && action == GLFW_PRESS){
    	keys = GLFW_KEY_W;
    }
    if (key == GLFW_KEY_S && action == GLFW_PRESS){
    	keys = GLFW_KEY_S;
    }
    if (key == GLFW_KEY_A && action == GLFW_PRESS){
    	keys = GLFW_KEY_A;
    }
    if (key == GLFW_KEY_D && action == GLFW_PRESS){
    	keys = GLFW_KEY_D;
    }
    if (key == GLFW_KEY_Q && action == GLFW_PRESS){
    	keys = GLFW_KEY_Q;
    }
    if (key == GLFW_KEY_E && action == GLFW_PRESS){
    	keys = GLFW_KEY_E;
    }
    if (key == GLFW_KEY_SPACE && action == GLFW_PRESS){
    	keys = GLFW_KEY_SPACE;
    }
    if (key == GLFW_KEY_LEFT_SHIFT && action == GLFW_PRESS){
    	keys = GLFW_KEY_LEFT_SHIFT;
    }
    if(action == GLFW_RELEASE)
    	keys = 0;
}

void process_movement(arcball_camera *camera) {
	if(keys == GLFW_KEY_W && actionPress == GLFW_PRESS)
        processKeyboard(camera, FORWARD, delta_time);
    if(keys == GLFW_KEY_S && actionPress == GLFW_PRESS)
        processKeyboard(camera, BACKWARD, delta_time);
    if(keys == GLFW_KEY_A && actionPress == GLFW_PRESS)
        processKeyboard(camera, LEFT, delta_time);
    if(keys == GLFW_KEY_D && actionPress == GLFW_PRESS)
        processKeyboard(camera, RIGHT, delta_time);
    if(keys == GLFW_KEY_SPACE && actionPress == GLFW_PRESS)
        processKeyboard(camera, UP, delta_time);
    if(keys == GLFW_KEY_LEFT_SHIFT && actionPress == GLFW_PRESS)
        processKeyboard(camera, DOWN, delta_time);
    if(keys == GLFW_KEY_Q && actionPress == GLFW_PRESS)
        processKeyboard(camera, LEFT_ROLL, delta_time);
    if(keys == GLFW_KEY_E && actionPress == GLFW_PRESS)
        processKeyboard(camera, RIGHT_ROLL, delta_time);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	processMouseScroll(&camera, yoffset, delta_time);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
	int state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
	//xpos = 1.0*xpos/getWindowWidth()*2 - 1.0;
	//ypos =  1.0*ypos/getWindowHeight()*2 - 1.0;
	if (state == GLFW_PRESS)
	{
		processMouseMovement(&camera, xpos, ypos, 0, delta_time);
	}
	else {
		processMouseMovement(&camera, xpos, ypos, 1, delta_time);
	}
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    camera.perspective_matrix = mat4Perspective(90.0, (float)width/(float)height, 0.1, 5000.0);
    printf("Framebuffer: %d, %d\n", width, height);
    glViewport(0, 0, width, height);
}


int run(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    //glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    //glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);

    /* Create a windowed mode window and its OpenGL context */
    glfwWindowHint( GLFW_DOUBLEBUFFER, GL_FALSE );
    const GLFWvidmode *mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
    window = glfwCreateWindow(mode->width, mode->height, "Cube", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }
    
    /* Make the window's context current */


    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    
    gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);

    fbo scene_frambuffer;
    framebuffer_init((float)mode->width, (float)mode->height, &scene_frambuffer);

    space_skybox = skybox_init(2048);

    arcball_camera_init(&camera, 90.0, (float)mode->width/(float)mode->height, 0.1, 5000.0);
    starsystem *sol = starsystem_init();
    player *player_1 = player_init();

	float lastFrame = 0.0;
    glViewport(0,0,mode->width,mode->height);
    glEnable(GL_CULL_FACE);
    glDisable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    //glEnable(GL_FRAMEBUFFER_SRGB); 
    /* Loop until the user closes the window */
    int frame_count = 0;
    float time_count = 0.0;

    while (!glfwWindowShouldClose(window))
    {
        frame_count++;
        GLfloat currentFrame = glfwGetTime();
        if(currentFrame - time_count >= 1.0) {
            printf("FPS: %d\n", frame_count);
            time_count = currentFrame;
            frame_count = 0;
        }
        
		delta_time = currentFrame - lastFrame;
		lastFrame = currentFrame;

        if (keys == GLFW_KEY_R && actionPress == GLFW_PRESS){
    	    reload_shader(sol);
        }

        /* Render here */
    
        int width, height;
        glfwGetWindowSize(window, &width, &height);

        process_movement(&camera);
        camera_update_view_matrix(&camera);
        
        glBindFramebuffer(GL_FRAMEBUFFER, scene_frambuffer.fbo_id);
            glEnable(GL_CULL_FACE);
            glEnable(GL_DEPTH_TEST);
            glClearColor(0.0f, 0.3f, 0.3f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            skybox_draw(space_skybox, camera);
        
            player_draw(player_1, camera, currentFrame, width, height, scene_frambuffer.fbo_id);
            starsystem_draw(sol, camera, currentFrame, width, height, scene_frambuffer.fbo_id);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        framebuffer_draw(&scene_frambuffer);

        /* Poll for and process events */
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwDestroyWindow(window);

    glfwTerminate();
    return 0;
}

int main(int argc, char *argv[]) {
    int exit_status = run();
    printf("exit(%d)\n", exit_status);
    return exit_status;

    //quadtree_point p;
    //p.x = 700.0;
    //p.y = 700.0;
    //quadtree_node *qt = quadtree_create(100.0, 100.0, 5, p);
    //printf("Quadtree created\n");
    //quadtree_search(qt);
    //return 0;
}
