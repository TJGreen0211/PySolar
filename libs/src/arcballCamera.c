#include "arcballCamera.h"

const double toRadians = M_PI / 180.0;

//static double Yaw = -90.0f;
//static double Pitch = 0.0f;
//static double MovementSpeed = 0.1f;
//static double maxSpeed = 2000.0f;
//static double MouseSensitivity = 0.6f;
//static double mouseZoom = 1.0f;

static double lastx, lasty;

//mat4 tr, rxry;
//vec3 rotation = {0.0, 0.0, 0.0};
//vec3 Position = {0.0, 1.0, 0.0};
//vec3 Up = {0.0, 0.0, 1.0};
//vec3 Front = {0.0, 0.0, -1.0};
//vec3 Right = {1.0, 0.0, 0.0};

mat4 getViewMatrix(ArcCamera *camera)
{
	double rad = 180.0 / M_PI;
	double arc_yaw, arcPitch;

	camera->translation_matrix = mat4Translate(camera->rotation.v[0]+camera->position.v[0], camera->rotation.v[1]+camera->position.v[1], camera->rotation.v[2]+camera->position.v[2], 0.0);
	vec3 d = {camera->rotation.v[0] - 0.0, camera->rotation.v[1] - 0.0, camera->rotation.v[2] - 0.0};
	d = vec3Normalize(d);
	arc_yaw = asin(-d.v[1]) * rad;
	arcPitch = atan2(d.v[0], -d.v[2]) * rad;
	//arcPitch = arcPitch - 180.0;
	camera->rotation_matrix = mat4Multiply(mat4RotateX(arc_yaw), mat4RotateY(arcPitch));

	//printf("DeltaTime: %f\n", arc_yaw);

	return mat4Multiply(camera->rotation_matrix, camera->translation_matrix);
}

vec4 getCameraPosition(ArcCamera camera, mat4 position) {
	mat4 mvTranspose = mat4Transpose(mat4Multiply(position, camera.translation_matrix));
	vec4 inverseCamera = {-mvTranspose.m[3][0], -mvTranspose.m[3][1], -mvTranspose.m[3][2], -mvTranspose.m[3][3]};
	vec4 camPosition = vec4MultiplyMat4(mvTranspose, inverseCamera);

	return camPosition;
}

void updateCameraVectors(ArcCamera *camera)
{
	camera->front.v[0] = camera->rotation_matrix.m[2][0];
	camera->front.v[1] = camera->rotation_matrix.m[2][1];
	camera->front.v[2] = camera->rotation_matrix.m[2][2];
	camera->front = vec3Normalize(camera->front);

	camera->right.v[0] = camera->rotation_matrix.m[0][0];
	camera->right.v[1] = camera->rotation_matrix.m[0][1];
	camera->right.v[2] = camera->rotation_matrix.m[0][2];
	camera->right = vec3Normalize(camera->right);

	camera->up.v[0] = camera->rotation_matrix.m[1][0];
	camera->up.v[1] = camera->rotation_matrix.m[1][1];
	camera->up.v[2] = camera->rotation_matrix.m[1][2];
	camera->up = vec3Normalize(camera->up);
}

void processKeyboard(ArcCamera *camera, enum Camera_Movement direction, double deltaTime)
{
	//if(deltaSpeed > maxSpeed)
	//	deltaSpeed = maxSpeed;
	double velocity = camera->movement_speed * deltaTime;// + deltaSpeed;

    if (direction == FORWARD)
        camera->position = vec3PlusEqual(camera->position, vec3ScalarMultiply(camera->front, velocity));
    if (direction == BACKWARD)
        camera->position = vec3MinusEqual(camera->position, vec3ScalarMultiply(camera->front, velocity));
    if (direction == LEFT)
        camera->position = vec3MinusEqual(camera->position, vec3ScalarMultiply(camera->right, velocity));
    if (direction == RIGHT)
        camera->position = vec3PlusEqual(camera->position, vec3ScalarMultiply(camera->right, velocity));
}

int constrainPitch;
void processMouseMovement(ArcCamera *camera, double xpos, double ypos, int reset_flag)
{
	vec3 mouse_arc = {xpos, ypos, 0.0};
	mouse_arc.v[1] = -mouse_arc.v[1];
	double mouse_arc2 = mouse_arc.v[0] * mouse_arc.v[0] + mouse_arc.v[1] * mouse_arc.v[1];

	if(mouse_arc2 <= 1*1)
		mouse_arc.v[2] = sqrt(1*1 - mouse_arc2);
	else
		mouse_arc = vec3Normalize(mouse_arc);

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

		if(camera->pitch > 159.0)
			camera->pitch = 159.0f;
		if(camera->pitch < 1.0)
			camera->pitch = 1.0f;

		camera->rotation.v[0] = camera->mouse_zoom * cos(camera->yaw/50.0) * sin(-camera->pitch/50.0);
		camera->rotation.v[1] = camera->mouse_zoom * cos(-camera->pitch/50.0);
		camera->rotation.v[2] = camera->mouse_zoom * sin(camera->yaw/50.0) * sin(-camera->pitch/50.0);

		updateCameraVectors(camera);
	}
}

double processMouseScroll(ArcCamera *camera, double yoffset)
{
	if (camera->mouse_zoom >= 1.0f && camera->mouse_zoom <= 20.0f)
        camera->mouse_zoom -= yoffset;
    if (camera->mouse_zoom <= 1.0f)
        camera->mouse_zoom = 1.0f;
    if (camera->mouse_zoom >= 20.0f)
         camera->mouse_zoom = 20.0f;

    camera->rotation.v[0] = camera->mouse_zoom * cos(camera->yaw/50.0) * sin(-camera->pitch/50.0);
	camera->rotation.v[1] = camera->mouse_zoom * cos(-camera->pitch/50.0);
	camera->rotation.v[2] = camera->mouse_zoom * sin(camera->yaw/50.0) * sin(-camera->pitch/50.0);

    return camera->mouse_zoom;
}
