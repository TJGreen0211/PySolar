#ifndef MATRIXMATH_H
#define MATRIXMATH_H
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

typedef struct matN {
	float **m;
} matN;

typedef struct mat4 {
	float m[4][4];
} mat4;

typedef struct mat3 {
	float m[3][3];
} mat3;

typedef struct mat2 {
	float m[2][2];
} mat2;

typedef struct vec4 {
	float v[4];
} vec4;

typedef struct vec3 {
	float v[3];
} vec3;

typedef struct vec2 {
	float v[2];
} vec2;

typedef struct quaternion {
	float v[3];
} quaternion;

mat4 mat4IdentityMatrix();
mat3 mat3IdentityMatrix();
mat2 mat2IdentityMatrix();

float quatLength(quaternion q);
quaternion quatNormalize(quaternion q);
quaternion quatConjugate(quaternion q);
quaternion quatMultiply(quaternion q, quaternion u);
mat4 quaternionToRotation(quaternion q);
quaternion angleAxis(float angle, vec3 axis, vec3 point);

vec4 vec4Add(vec4 v, vec4 u);
vec3 vec3Add(vec3 v, vec3 u);
vec2 vec2Add(vec2 v, vec2 u);

vec4 vec4ScalarMultiply(vec4 v, float u);
vec3 vec3ScalarMultiply(vec3 v, float u);
vec2 vec2ScalarMultiply(vec2 v, float u);

vec4 vec4Divide(vec4 v, float u);
vec3 vec3Divide(vec3 v, float u);
vec2 vec2Divide(vec2 v, float u);

vec4 vec4PlusEqual(vec4 v, vec4 u);
vec3 vec3PlusEqual(vec3 v, vec3 u);
vec2 vec2PlusEqual(vec2 v, vec2 u);

vec4 vec4MinusEqual(vec4 v, vec4 u);
vec3 vec3MinusEqual(vec3 v, vec3 u);
vec2 vec2MinusEqual(vec2 v, vec2 u);

float vec4Dot(vec4 u, vec4 v);
float vec3Dot(vec3 u, vec3 v);
float vec2Dot(vec2 u, vec2 v);

float vec4Length(vec4 v);
float vec3Length(vec3 v);
float vec2Length(vec2 v);

vec4 vec4Normalize(vec4 v);
vec3 vec3Normalize(vec3 v);
vec2 vec2Normalize(vec2 v);

//Not technically defined in R4 but  still useful
vec4 vec4crossProduct(vec4 v, vec4 u);
vec3 crossProduct(vec3 v, vec3 u);

vec4 vec4Multiply(vec4 v, vec4 u);
vec3 vec3Multiply(vec3 v, vec3 u);
vec2 vec2Multiply(vec2 v, vec2 u);

mat4 mat4Rotate(float x, float y, float z);
mat4 mat4RotateX(float theta);
mat4 mat4RotateY(float theta);
mat4 mat4RotateZ(float theta);

mat4 mat4Translate(float x, float y, float z, float w);
mat4 mat4TranslateVec4( vec4 v );
mat3 mat3Translate(float x, float y, float z);
mat3 mat3TranslateVec3( vec3 v );
mat2 mat2Translate(float x, float y);
mat2 mat2TranslateVec2( vec2 v );

mat4 mat4Scale(float x, float y, float z, float w);
mat4 mat4ScaleVec4(vec4 v);
mat4 mat4ScaleScalar(float value);

mat3 mat3Scale(float x, float y, float z);
mat3 mat3ScaleVec3(vec3 v);
mat3 mat3ScaleScalar(float value);

mat2 mat2Scale(float x, float y);
mat2 mat2ScaleVec4(vec4 v);
mat2 mat2ScaleScalar(float value);

mat4 mat4Multiply(mat4 c, mat4 d);
vec4 vec4MultiplyMat4(mat4 a, vec4 v);
vec3 vec3MultiplyMat3(mat3 a, vec3 v);
vec2 vec2MultiplyMat2(mat2 a, vec2 v);

mat4 mat4Transpose(mat4 a);
mat3 mat3Transpose(mat3 a);
mat2 mat2Transpose(mat2 a);

mat4 mat4Perspective(float fovy, float aspect, float zNear, float zFar);
mat4 mat4Frustum(float left, float right, float bottom, float top, float zNear, float zFar);
mat4 mat4Ortho(float left, float right, float bottom, float top, float zNear, float zFar );
mat4 mat4LookAt(vec3 eye, vec3 at, vec3 up);

void vec4Print(vec4 v);
void vec3Print(vec3 v);
void vec2Print(vec2 v);

void mat4Print(mat4 m);

#endif
