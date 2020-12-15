#include "player.h"

player *player_init() {
    player *p = (player *)malloc(sizeof(player));

    p->mesh_shader = shader_create_program("../shaders/mesh.vert",
        "../shaders/mesh.frag",NULL,NULL,NULL);
    geometry mesh;
    geometry_load_object_file("../resources/models/ship.obj", &mesh);
    p->mesh_buffer = buffer_init(mesh);
    geometry_mesh_dealloc(&mesh);

    p->current_position.v[0] = 0.0; p->current_position.v[1] = 0.0; p->current_position.v[2] = 0.0;
    p->delta_position.v[0] = 0.0; p->delta_position.v[1] = 0.0; p->delta_position.v[2] = 0.0;
    p->current_rotation = 0.0;

    return p;
}

void player_draw(player *p, arcball_camera camera, float time, int width, int height, unsigned int framebuffer) {
    glUseProgram(p->mesh_shader);

    vec3 last_position = p->current_position;
    p->current_position = camera.position;
    p->delta_position.v[0] = last_position.v[0] - p->current_position.v[0];
    p->delta_position.v[1] = last_position.v[1] - p->current_position.v[1];
    p->delta_position.v[2] = last_position.v[2] - p->current_position.v[2];

    vec3 delta_normalized = vec3Normalize(p->delta_position);
    
    float rotation_speed = 5.0;
    float movement_rotation_angle = atan2(delta_normalized.v[0],delta_normalized.v[2])*(180.0 / M_PI);
    if(movement_rotation_angle < 0.0) {
        if(p->current_rotation > movement_rotation_angle) {
            p->current_rotation -= 1.0*rotation_speed;
            //printf("-: %f", p->current_rotation);
        }
    }
    if(movement_rotation_angle > 0.0) {
        if(p->current_rotation < movement_rotation_angle) {
            p->current_rotation += 1.0*rotation_speed;
            //printf("+: %f", p->current_rotation);
        }
    }
    
    mat4 model = mat4Multiply(mat4Translate(camera.position.v[0], camera.position.v[1], camera.position.v[2], 1.0), mat4RotateY(p->current_rotation-180.0) );//mat4ScaleScalar(30.0);

    glUniformMatrix4fv(glGetUniformLocation(p->mesh_shader, "projection"), 1, GL_FALSE, &camera.perspective_matrix.m[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(p->mesh_shader, "view"), 1, GL_FALSE, &camera.view_matrix.m[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(p->mesh_shader, "model"), 1, GL_FALSE, &model.m[0][0]);

    vec4 camera_position = getCameraPosition(camera, model);
    glUniform3f(glGetUniformLocation(p->mesh_shader, "camera_position"), camera_position.v[0], camera_position.v[1], camera_position.v[2]);
    glUniform3f(glGetUniformLocation(p->mesh_shader, "lightPosition"), 10.0, 5.0, -4.0);

    glBindVertexArray(p->mesh_buffer.vao);
	glDrawArrays(GL_TRIANGLES, 0,  p->mesh_buffer.vertex_number);
    glBindVertexArray(0);
}