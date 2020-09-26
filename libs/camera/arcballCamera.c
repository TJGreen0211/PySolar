#include "arcballCamera.h"

const double toRadians = M_PI / 180.0;
static double lastx, lasty;

mat4 getViewMatrix(ArcCamera *camera)
{
    double rad = 180.0 / M_PI;
    double arc_yaw, arc_pitch;

    //Lock Y direction
    camera->translation_matrix = mat4Translate(camera->front.v[0]-camera->position.v[0], 0.0, camera->front.v[2]-camera->position.v[2], 0.0);
    vec3 d = {{camera->front.v[0] - 0.0, camera->front.v[1] - 0.0, camera->front.v[2] - 0.0}};
    d = vec3Normalize(d);
    arc_yaw = asin(-d.v[1]) * rad;
    arc_pitch = atan2(d.v[0], -d.v[2]) * rad;
    //printf("Yaw: %f, Pitch: %f\n", arc_yaw, arc_pitch);
    camera->rotation_matrix = mat4Multiply(mat4RotateX(arc_yaw), mat4RotateY(arc_pitch));

    return mat4Multiply(camera->rotation_matrix, camera->translation_matrix);
}

vec4 getCameraPosition(ArcCamera camera, mat4 position) {
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

static void updateCameraVectors(ArcCamera *camera)
{
    double rad = 180.0 / M_PI;

    camera->front.v[0] = cos(camera->yaw * rad) * cos(camera->pitch* rad);
    camera->front.v[1] = sin(camera->pitch* rad);
    camera->front.v[2] = sin(camera->yaw * rad) * cos(camera->pitch* rad);
    camera->front = vec3Normalize(camera->front);

    camera->right = vec3Normalize(crossProduct(camera->front, camera->up));
    camera->up = vec3Normalize(crossProduct(camera->right, camera->front));
}

void processKeyboard(ArcCamera *camera, enum Camera_Movement direction, double deltaTime)
{
    double velocity = camera->movement_speed * deltaTime;
    if(camera->right.v[1] == 0.0) {
        camera->right.v[1] = 0.1;
    }

    camera->right.v[0] = camera->right.v[0] == 0.0 ? 0.1 : camera->right.v[0];
    camera->right.v[1] = camera->right.v[1] == 0.0 ? 0.1 : camera->right.v[1];
    camera->right.v[2] = camera->right.v[2] == 0.0 ? 0.1 : camera->right.v[2];
    
    camera->front.v[0] = camera->front.v[0] == 0.0 ? 0.1 : camera->front.v[0];
    camera->front.v[1] = camera->front.v[1] == 0.0 ? 0.1 : camera->front.v[1];
    camera->front.v[2] = camera->front.v[2] == 0.0 ? 0.1 : camera->front.v[2];

    if (direction == FORWARD) {
        camera->position = vec3PlusEqual(camera->position, vec3ScalarMultiply(camera->front, velocity));
    }
    if (direction == BACKWARD) {
        camera->position = vec3MinusEqual(camera->position, vec3ScalarMultiply(camera->front, velocity));
    }
    if (direction == LEFT) {
        camera->position = vec3PlusEqual(camera->position, vec3ScalarMultiply(camera->right, velocity));
    }
    if (direction == RIGHT) {
        camera->position = vec3MinusEqual(camera->position, vec3ScalarMultiply(camera->right, velocity));
    }
}

void processMouseMovement(ArcCamera *camera, double xpos, double ypos, int reset_flag)
{
    if(reset_flag) {
        lastx = xpos;
        lasty = ypos;
    }
    else {

        double diffx = xpos - lastx;
        double diffy = ypos - lasty;

        lastx = xpos;
        lasty = ypos;

        diffx *= camera->mouse_sensitivity;
        diffy *= camera->mouse_sensitivity;

        camera->yaw += diffx;
        camera->pitch += diffy;

        if (camera->pitch > 89.0f) {
            camera->pitch = 89.0f;
        }
        if (camera->pitch < -89.0f) {
            camera->pitch = -89.0f;
        }

        updateCameraVectors(camera);
    }
}

void processMouseScroll(ArcCamera *camera, double yoffset)
{
    if (camera->mouse_zoom >= 1.0f && camera->mouse_zoom <= 20.0f)
        camera->mouse_zoom -= yoffset;
    if (camera->mouse_zoom <= 1.0f)
        camera->mouse_zoom = 1.0f;
    if (camera->mouse_zoom >= 20.0f)
         camera->mouse_zoom = 20.0f;
}
