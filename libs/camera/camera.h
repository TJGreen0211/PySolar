#ifndef CAMERA_H
#define CAMERA_H

#ifdef _WIN32
	#define DLL_EXPORT __declspec(dllexport)
#else
	#define DLL_EXPORT
#endif


#include "../matrixmath/matrixMath.h"

typedef struct arcball_camera {
	mat4 view_matrix;
	mat4 translation_matrix;
	mat4 rotation_matrix;
	mat4 velocity_matrix;
	mat4 perspective_matrix;
	vec3 rotation;
	vec3 position;
	vec3 up;
	vec3 front;
	vec3 right;
	vec3 velocity;

	float yaw;
	float pitch;
	float roll;
	float movement_speed;
	float max_speed;
	float mouse_sensitivity;
	float mouse_zoom;
	float zoom_speed;
} arcball_camera;

enum Camera_Movement {
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT,
	UP,
	DOWN,
	LEFT_ROLL,
	RIGHT_ROLL
};

void camera_update_view_matrix(arcball_camera *camera);
vec4 getCameraPosition(arcball_camera camera, mat4 position);

void setCameraPosition(arcball_camera *camera, vec3 position);
void processKeyboard(arcball_camera *camera, enum Camera_Movement direction, float delta_time);
void processMouseMovement(arcball_camera *camera, float xpos, float ypos, int resetFlag, float delta_time);
void processMouseScroll(arcball_camera *camera, float yoffset, float delta_time);
void arcball_camera_init(arcball_camera *camera, float fov, float aspect, float z_near, float z_far);

#endif
