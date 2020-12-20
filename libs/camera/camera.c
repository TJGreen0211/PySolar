#include "camera.h"

const float toRadians = M_PI / 180.0;
static float lastx, lasty;

void arcball_camera_init(arcball_camera *camera, float fov, float aspect, float z_near, float z_far) {
	vec3 init_zero = {{0.0, 0.0, 0.0}};
	vec3 init_right = {{1.0, 0.0, 0.0}};
	vec3 init_front = {{0.0, 1.0, 0.0}};
	vec3 init_up = {{0.0, 0.0, 1.0}};
	vec3 init_position = {{600.0, 0.0, 0.0}};
	mat4 init_identity = mat4IdentityMatrix();

    camera->perspective_matrix = mat4Perspective( fov, aspect, z_near, z_far);
	camera->translation_matrix = init_identity;
	camera->rotation_matrix = init_identity;
	camera->velocity_matrix = init_identity;
	camera->rotation = init_up;
	camera->position = init_zero;
	camera->up = init_up;
	camera->front = init_front;
	camera->right = init_right;
	camera->position = init_position;
    camera->velocity = init_zero;

	camera->yaw = -0.0;
	camera->pitch =0.0;
    camera->roll = 0.0;
	camera->movement_speed = 100.1;
	camera->max_speed = 2000.0;
	camera->mouse_sensitivity = 0.25;
	camera->mouse_zoom = 1.0;
    camera->zoom_speed = 20.0;
}

void camera_update_view_matrix(arcball_camera *camera)
{
    camera->translation_matrix = mat4Translate(camera->mouse_zoom * camera->front.v[0]-camera->position.v[0], camera->mouse_zoom * camera->front.v[1]-camera->position.v[1], camera->mouse_zoom * camera->front.v[2]-camera->position.v[2], 0.0);
    camera->view_matrix = mat4Multiply(camera->rotation_matrix, camera->translation_matrix);
}

vec4 getCameraPosition(arcball_camera camera, mat4 position) {
    mat4 p = mat4IdentityMatrix();
    //Need only the position part of the matrix
    p.m[0][3] = position.m[0][3];
    p.m[1][3] = position.m[1][3];
    p.m[2][3] = position.m[2][3];
    mat4 mvTranspose = mat4Transpose(mat4Multiply(p, camera.translation_matrix));
    vec4 inverseCamera = {{-mvTranspose.m[3][0], -mvTranspose.m[3][1], -mvTranspose.m[3][2], -mvTranspose.m[3][3]}};
    vec4 camPosition = vec4MultiplyMat4(mvTranspose, inverseCamera);

    return camPosition;
}

void setCameraPosition(arcball_camera *camera, vec3 position) {
    camera->position.v[0] = position.v[0];
    camera->position.v[1] = position.v[1];
    camera->position.v[2] = position.v[2];
}

static void updateCameraVectors(arcball_camera *camera)
{
    float rad = 180.0 / M_PI;

    camera->front.v[0] = cos(camera->yaw) * cos(camera->pitch);
    camera->front.v[1] = sin(camera->pitch);
    camera->front.v[2] = sin(camera->yaw ) * cos(camera->pitch);
    camera->front = vec3Normalize(camera->front);

    float arc_yaw = asin(-camera->front.v[1]) * rad;
    float arc_pitch = atan2(camera->front.v[0], -camera->front.v[2]) * rad;
    camera->rotation_matrix = mat4Multiply(mat4RotateX(arc_yaw), mat4RotateY(arc_pitch));

    camera->right.v[0] = camera->rotation_matrix.m[0][0];    
    camera->right.v[1] = camera->rotation_matrix.m[0][1];
    camera->right.v[2] = camera->rotation_matrix.m[0][2];
    camera->right = vec3Normalize(camera->right);

    camera->up.v[0] = camera->rotation_matrix.m[1][0];    
    camera->up.v[1] = camera->rotation_matrix.m[1][1];
    camera->up.v[2] = camera->rotation_matrix.m[1][2];
    camera->up = vec3Normalize(camera->up);

    //camera->right = vec3Normalize(crossProduct(camera->front, camera->up));
    //camera->up = vec3Normalize(crossProduct(camera->right, camera->front));
}

void processKeyboard(arcball_camera *camera, enum Camera_Movement direction, float delta_time)
{
    float velocity = camera->movement_speed * delta_time;
    if(camera->right.v[1] == 0.0) {
        camera->right.v[1] = 0.1;
    }

    camera->right.v[0] = camera->right.v[0] == 0.0 ? 0.1 : camera->right.v[0];
    camera->right.v[1] = camera->right.v[1] == 0.0 ? 0.1 : camera->right.v[1];
    camera->right.v[2] = camera->right.v[2] == 0.0 ? 0.1 : camera->right.v[2];
    
    camera->front.v[0] = camera->front.v[0] == 0.0 ? 0.1 : camera->front.v[0];
    camera->front.v[1] = camera->front.v[1] == 0.0 ? 0.1 : camera->front.v[1];
    camera->front.v[2] = camera->front.v[2] == 0.0 ? 0.1 : camera->front.v[2];

    camera->velocity.v[0] = camera->position.v[0];
    camera->velocity.v[1] = camera->position.v[1];
    camera->velocity.v[2] = camera->position.v[2];

    //printf("Start: x: %f, y: %f, z: %f\n", camera->position.v[0], camera->position.v[1], camera->position.v[2]);

    if (direction == FORWARD) {
        camera->position = vec3PlusEqual(camera->position, vec3ScalarMultiply(camera->front, velocity));
    }
    if (direction == BACKWARD) {
        camera->position = vec3MinusEqual(camera->position, vec3ScalarMultiply(camera->front, velocity));
    }
    if (direction == LEFT) {
        camera->position = vec3MinusEqual(camera->position, vec3ScalarMultiply(camera->right, velocity));
    }
    if (direction == RIGHT) {
        camera->position = vec3PlusEqual(camera->position, vec3ScalarMultiply(camera->right, velocity));
    }
    if (direction == UP) {
        camera->position = vec3PlusEqual(camera->position, vec3ScalarMultiply(camera->up, velocity));
    }
    if (direction == DOWN) {
        camera->position = vec3MinusEqual(camera->position, vec3ScalarMultiply(camera->up, velocity));
    }

    if (direction == LEFT_ROLL) {
        camera->roll += delta_time*10.0;
        updateCameraVectors(camera);
    }
    if (direction == RIGHT_ROLL) {
        camera->roll -= delta_time*10.0;
        updateCameraVectors(camera);
    }

    //printf("End: x: %f, y: %f, z: %f\n", camera->position.v[0], camera->position.v[1], camera->position.v[2]);

    camera->velocity.v[0] = camera->position.v[0] - camera->velocity.v[0];
    camera->velocity.v[1] = camera->position.v[1] - camera->velocity.v[1];
    camera->velocity.v[2] = camera->position.v[2] - camera->velocity.v[2];
    camera->velocity = vec3Normalize(camera->velocity);
}

void processMouseMovement(arcball_camera *camera, float xpos, float ypos, int reset_flag, float delta_time)
{
    if(reset_flag) {
        lastx = xpos;
        lasty = ypos;
    }
    else {

        float diffx = xpos - lastx;
        float diffy = ypos - lasty;

        lastx = xpos;
        lasty = ypos;

        diffx *= camera->mouse_sensitivity*delta_time;
        diffy *= camera->mouse_sensitivity*delta_time;

        camera->yaw += diffx;
        camera->pitch += diffy;

        /*if (camera->pitch > 89.0f) {
            camera->pitch = 89.0f;
        }
        if (camera->pitch < -89.0f) {
            camera->pitch = -89.0f;
        }*/
        
        updateCameraVectors(camera);
    }
}

void processMouseScroll(arcball_camera *camera, float yoffset, float delta_time)
{
    if (camera->mouse_zoom <= 1.0f)
        camera->mouse_zoom = 1.0f;
    if (camera->mouse_zoom >= 20.0f)
         camera->mouse_zoom = 20.0f;

    camera->mouse_zoom += yoffset*delta_time*camera->zoom_speed;

}
