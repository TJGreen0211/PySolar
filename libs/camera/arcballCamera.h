#ifndef ARCBALLCAMERA_H
#define ARCBALLCAMERA_H

#include "../src/matrixMath.h"

typedef struct ArcCamera {
	mat4 translation_matrix;
	mat4 rotation_matrix;
	vec3 rotation;
	vec3 position;
	vec3 up;
	vec3 front;
	vec3 right;

	double yaw;
	double pitch;
	double movement_speed;
	double max_speed;
	double mouse_sensitivity;
	double mouse_zoom;
} ArcCamera;

enum Camera_Movement {
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT
};

mat4 getViewMatrix(ArcCamera *camera);
vec4 getCameraPosition(ArcCamera camera, mat4 position);

void setCameraPosition(ArcCamera *camera, vec3 position);
void processKeyboard(ArcCamera *camera, enum Camera_Movement direction, double deltaTime);
void processMouseMovement(ArcCamera *camera, double xpos, double ypos, int resetFlag);
void processMouseScroll(ArcCamera *camera, double yoffset);

#endif
