/*
	* Author: Tyler Green
	Helper module for matrix math.

*/

#include "matrixMath.h"

#define DLL_EXPORT __declspec(dllexport)

// Converts degrees to radians.
#define DegreesToRadians (M_PI / 180.0)

mat4 MAT4_IDENTITY_MATRIX = {
	{{1.0, 0.0, 0.0, 0.0},
	{0.0, 1.0, 0.0, 0.0},
	{0.0, 0.0, 1.0, 0.0},
	{0.0, 0.0, 0.0, 1.0}}
};

mat4 MAT4_ZERO_MATRIX = {{
	{0.0, 0.0, 0.0, 0.0},
	{0.0, 0.0, 0.0, 0.0},
	{0.0, 0.0, 0.0, 0.0},
	{0.0, 0.0, 0.0, 0.0}
}};


mat3 MAT3_IDENTITY_MATRIX = {{
	{0.0, 0.0, 0.0},
	{0.0, 0.0, 0.0},
	{0.0, 0.0, 0.0}
}};

mat3 MAT3_ZERO_MATRIX = {{
	{0.0, 0.0, 0.0},
	{0.0, 0.0, 0.0},
	{0.0, 0.0, 0.0}
}};

mat2 MAT2_IDENTITY_MATRIX = {{
	{0.0, 0.0},
	{0.0, 0.0}
}};

mat2 MAT2_ZERO_MATRIX = {{
	{0.0, 0.0},
	{0.0, 0.0}
}};

mat4 mat4IdentityMatrix() {
	return MAT4_IDENTITY_MATRIX;
}

mat3 mat3IdentityMatrix() {
	return MAT3_IDENTITY_MATRIX;
}

mat2 mat2IdentityMatrix() {
	return MAT2_IDENTITY_MATRIX;
}

float quatLength(quaternion q) {
	return sqrt(q.v[0]*q.v[0] + q.v[1]*q.v[1] + q.v[2]*q.v[2] + q.v[3]*q.v[3]);
}

quaternion quatNormalize(quaternion q) {
	float len = quatLength(q);
	q.v[0] /= len;
	q.v[1] /= len;
	q.v[2] /= len;
	q.v[3] /= len;

	return q;
}

quaternion quatConjugate(quaternion q) {
	q.v[2] = q.v[2];
	q.v[0] = -q.v[0];
	q.v[1] = -q.v[1];
	q.v[2] = -q.v[2];
	return q;
}

quaternion quatMultiply(quaternion q, quaternion u) {
	quaternion c;
	c.v[3] = q.v[3]*u.v[3] - q.v[0]*u.v[0] - q.v[1]*u.v[1] - q.v[2]*u.v[2];
	c.v[0] = q.v[3]*u.v[0] + q.v[0]*u.v[3] + q.v[1]*u.v[2] - q.v[2]*u.v[1];
	c.v[1] = q.v[3]*u.v[1] - q.v[0]*u.v[2] + q.v[1]*u.v[3] + q.v[2]*u.v[0];
	c.v[2] = q.v[3]*u.v[2] + q.v[0]*u.v[1] - q.v[1]*u.v[0] + q.v[2]*u.v[3];
	return c;
}

mat4 quaternionToRotation(quaternion q) {
	mat4 rotation = {
		{
		{1.0-2.0*(q.v[1]*q.v[1] + q.v[2]*q.v[2]), 	  2.0*(q.v[0]*q.v[1] + q.v[2]*q.v[3]), 	   2.0*(q.v[0]*q.v[2] - q.v[1]*q.v[3]), 0.0},
		{	 2.0*(q.v[0]*q.v[1] - q.v[2]*q.v[3]), 1.0-2.0*(q.v[0]*q.v[0] + q.v[2]*q.v[2]), 	   2.0*(q.v[1]*q.v[2] + q.v[0]*q.v[3]), 0.0},
		{    2.0*(q.v[0]*q.v[2] + q.v[1]*q.v[3]),     2.0*(q.v[1]*q.v[2] - q.v[0]*q.v[3]), 1.0-2.0*(q.v[0]*q.v[0] + q.v[1]*q.v[1]), 0.0},
		{	 0.0, 						  0.0, 					   0.0, 						1.0}
		}
	};
	return rotation;
}

quaternion angleAxis(float angle, vec3 axis, vec3 point) {
	quaternion r, p;
	r.v[3] = cos(angle/2.0);
	r.v[0] = axis.v[0] * sin(angle/2.0);
	r.v[1] = axis.v[1] * sin(angle/2.0);
	r.v[2] = axis.v[2] * sin(angle/2.0);

	p.v[3] = 0.0;
	p.v[0] = point.v[0];
	p.v[1] = point.v[1];
	p.v[2] = point.v[2];

	return quatMultiply(quatMultiply(r, p), quatConjugate(r));
}

DLL_EXPORT vec4 vec4Add(vec4 v, vec4 u)
{
	vec4 temp = {{v.v[0] + u.v[0], v.v[1] + u.v[1], v.v[2] + u.v[2], v.v[3] + u.v[3],}};
	return temp;
}

DLL_EXPORT vec3 vec3Add(vec3 v, vec3 u)
{
	vec3 temp = {{v.v[0] + u.v[0], v.v[1] + u.v[1], v.v[2] + u.v[2]}};
	return temp;
}

DLL_EXPORT vec2 vec2Add(vec2 v, vec2 u)
{
	vec2 temp = {{v.v[0] + u.v[0], v.v[1] + u.v[1] }};
	return temp;
}

DLL_EXPORT vec4 vec4ScalarMultiply(vec4 v, float u)
{
	vec4 temp = {{v.v[0] * u, v.v[1] * u, v.v[2] * u, v.v[3] *u}};
	return temp;
}

DLL_EXPORT vec3 vec3ScalarMultiply(vec3 v, float u)
{
	vec3 temp = {{v.v[0] * u, v.v[1] * u, v.v[2] * u}};
	return temp;
}

DLL_EXPORT vec2 vec2ScalarMultiply(vec2 v, float u)
{
	vec2 temp = {{v.v[0] * u, v.v[1] * u}};
	return temp;
}

DLL_EXPORT vec4 vec4Divide(vec4 v, float u)
{
	vec4 temp = {{v.v[0] / u, v.v[1] / u, v.v[2] / u, v.v[3] / u}};
	return temp;
}

DLL_EXPORT vec3 vec3Divide(vec3 v, float u)
{
	vec3 temp = {{v.v[0] / u, v.v[1] / u, v.v[2] / u}};
	return temp;
}

DLL_EXPORT vec2 vec2Divide(vec2 v, float u)
{
	vec2 temp = {{v.v[0] / u, v.v[1] / u}};
	return temp;
}

DLL_EXPORT vec4 vec4PlusEqual(vec4 v, vec4 u)
{
	vec4 temp = {{v.v[0] += u.v[0], v.v[1] += u.v[1], v.v[2] += u.v[2], v.v[3] += u.v[3]}};
	return temp;
}

DLL_EXPORT vec3 vec3PlusEqual(vec3 v, vec3 u)
{
	vec3 temp = {{v.v[0] += u.v[0], v.v[1] += u.v[1], v.v[2] += u.v[2]}};
	return temp;
}

DLL_EXPORT vec2 vec2PlusEqual(vec2 v, vec2 u)
{
	vec2 temp = {{v.v[0] += u.v[0], v.v[1] += u.v[1]}};
	return temp;
}

DLL_EXPORT vec4 vec4MinusEqual(vec4 v, vec4 u)
{
	vec4 temp = {{v.v[0] -= u.v[0], v.v[1] -= u.v[1], v.v[2] -= u.v[2], v.v[3] -= u.v[3]}};
	return temp;
}

DLL_EXPORT vec3 vec3MinusEqual(vec3 v, vec3 u)
{
	vec3 temp = {{v.v[0] -= u.v[0], v.v[1] -= u.v[1], v.v[2] -= u.v[2]}};
	return temp;
}

DLL_EXPORT vec2 vec2MinusEqual(vec2 v, vec2 u)
{
	vec2 temp = {{v.v[0] -= u.v[0], v.v[1] -= u.v[1]}};
	return temp;
}

DLL_EXPORT float vec4Dot(vec4 u, vec4 v) {
	return u.v[0]*v.v[0] + u.v[1]*v.v[1] + u.v[2]*v.v[2] + u.v[3]*v.v[3];
}

DLL_EXPORT float vec3Dot(vec3 u, vec3 v) {
	return u.v[0]*v.v[0] + u.v[1]*v.v[1] + u.v[2]*v.v[2];
}

DLL_EXPORT float vec2Dot(vec2 u, vec2 v) {
	return u.v[0]*v.v[0] + u.v[1]*v.v[1];
}

DLL_EXPORT float vec4Length(vec4 v) {
	return sqrt(vec4Dot(v, v));
}

DLL_EXPORT float vec3Length(vec3 v) {
	return sqrt(vec3Dot(v, v));
}

DLL_EXPORT float vec2Length(vec2 v) {
	return sqrt(vec2Dot(v, v));
}

DLL_EXPORT vec4 vec4Normalize(vec4 v) {
	float vecLength = vec4Length(v);
	vec4 dividebyzero = {{0.0, 0.0, 0.0, 0.0}};
	if (vecLength == 0)
		return dividebyzero;
	float X, Y, Z;
	X = v.v[0] / vecLength;
	Y = v.v[1] / vecLength;
	Z = v.v[2] / vecLength;
	vec4 u = {{X, Y, Z, v.v[3]}};
	return u;
}

DLL_EXPORT vec3 vec3Normalize(vec3 v) {
	float vecLength = vec3Length(v);
	vec3 dividebyzero = {{0.0, 0.0, 0.0}};
	if (vecLength == 0)
		return dividebyzero;
	float X, Y, Z;
	X = v.v[0] / vecLength;
	Y = v.v[1] / vecLength;
	Z = v.v[2] / vecLength;
	vec3 u = {{X, Y, Z}};
	return u;
}

DLL_EXPORT vec2 vec2Normalize(vec2 v) {
	float vecLength = vec2Length(v);
	vec2 dividebyzero = {{0.0, 0.0}};
	if (vecLength == 0)
		return dividebyzero;
	float X, Y;
	X = v.v[0] / vecLength;
	Y = v.v[1] / vecLength;
	vec2 u = {{X, Y}};
	return u;
}

DLL_EXPORT vec4 vec4CrossProduct(vec4 v, vec4 u) {
	vec4 out = {{v.v[1] * u.v[2] - v.v[2] * u.v[1],
				v.v[2] * u.v[0] - v.v[0] * u.v[2],
				v.v[0] * u.v[1] - v.v[1] * u.v[0],
				0.0}};
	return out;
}

DLL_EXPORT vec3 crossProduct(vec3 v, vec3 u) {
	vec3 out = {{v.v[1] * u.v[2] - v.v[2] * u.v[1],
				v.v[2] * u.v[0] - v.v[0] * u.v[2],
				v.v[0] * u.v[1] - v.v[1] * u.v[0]}};
	return out;
}

DLL_EXPORT vec4 vec4Multiply(vec4 v, vec4 u)
{
	vec4 out = {{ u.v[0]*v.v[0], u.v[1]*v.v[1], u.v[2]*v.v[2], u.v[3]*v.v[3] }};
	return out;
}

DLL_EXPORT vec3 vec3Multiply(vec3 v, vec3 u)
{
	vec3 out = {{ u.v[0]*v.v[0], u.v[1]*v.v[1], u.v[2]*v.v[2] }};
	return out;
}

DLL_EXPORT vec2 vec2Multiply(vec2 v, vec2 u)
{
	vec2 out = {{ u.v[0]*v.v[0], u.v[1]*v.v[1] }};
	return out;
}

DLL_EXPORT mat4 mat4Rotate(float x, float y, float z) {
	float a = DegreesToRadians * x;
	float b = DegreesToRadians * y;
	float c = DegreesToRadians * z;
	mat4 r = MAT4_IDENTITY_MATRIX;
	r.m[0][0] = cos(b)*cos(c);
	r.m[0][1] = -(cos(b)*sin(c));
	r.m[0][2] = sin(b);
	r.m[1][0] = (sin(a)*sin(b)*cos(c))+(cos(a)*sin(c));
	r.m[1][1] = -(sin(a)*sin(b)*sin(c))+(cos(a)*cos(c));
	r.m[1][2] = -(sin(a)*cos(b));
	r.m[2][0] = -(cos(a)*sin(b)*cos(c))+(sin(a)*sin(c));
	r.m[2][1] = (cos(a)*sin(b)*sin(c))+(sin(a)*cos(c));
	r.m[2][2] = cos(a)*cos(b);

	return r;
}

DLL_EXPORT mat4 mat4RotateX(float theta)
{
	float angle = DegreesToRadians * theta;
	mat4 rotation = MAT4_IDENTITY_MATRIX;
	rotation.m[2][2] = rotation.m[1][1] = cos(angle);
	rotation.m[2][1] = sin(angle);
	rotation.m[1][2] = -rotation.m[2][1];
	return rotation;
}

DLL_EXPORT mat4 mat4RotateY(float theta)
{
	float angle = DegreesToRadians * theta;
	mat4 rotation = MAT4_IDENTITY_MATRIX;
	rotation.m[2][2] = rotation.m[0][0] = cos(angle);
	rotation.m[0][2] = sin(angle);
	rotation.m[2][0] = -rotation.m[0][2];
	return rotation;
}

DLL_EXPORT mat4 mat4RotateZ(float theta)
{
	float angle = DegreesToRadians * theta;
	mat4 rotation = MAT4_IDENTITY_MATRIX;
	rotation.m[0][0] = rotation.m[1][1] = cos(angle);
	rotation.m[1][0] = sin(angle);
	rotation.m[0][1] = -rotation.m[1][0];
	return rotation;
}

DLL_EXPORT mat4 mat4Translate(float x, float y, float z, float w)
{
	mat4 c = MAT4_IDENTITY_MATRIX;
	c.m[0][3] = x;
	c.m[1][3] = y;
	c.m[2][3] = z;
	return c;
}

DLL_EXPORT mat4 mat4TranslateVec4( vec4 v )
{
    return mat4Translate( v.v[0], v.v[1], v.v[2], v.v[3]);
}

DLL_EXPORT mat3 mat3Translate(float x, float y, float z)
{
	mat3 c = MAT3_IDENTITY_MATRIX;
	c.m[0][3] = x;
	c.m[1][3] = y;
	c.m[2][3] = z;
	return c;
}

DLL_EXPORT mat3 mat3TranslateVec3( vec3 v )
{
    return mat3Translate( v.v[0], v.v[1], v.v[2]);
}

DLL_EXPORT mat2 mat2Translate(float x, float y)
{
	mat2 c = MAT2_IDENTITY_MATRIX;
	c.m[0][3] = x;
	c.m[1][3] = y;
	return c;
}

DLL_EXPORT mat2 mat2TranslateVec2( vec2 v )
{
    return mat2Translate( v.v[0], v.v[1]);
}

DLL_EXPORT mat4 mat4Scale(float x, float y, float z, float w)
{
	mat4 c = MAT4_IDENTITY_MATRIX;
	c.m[0][0] = x;
	c.m[1][1] = y;
	c.m[2][2] = z;
	return c;
}

DLL_EXPORT mat4 mat4ScaleVec4(vec4 v)
{
	return mat4Scale(v.v[0], v.v[1], v.v[2], v.v[3]);
}

DLL_EXPORT mat4 mat4ScaleScalar(float value)
{
	return mat4Scale(value, value, value, 1.0);
}

DLL_EXPORT mat3 mat3Scale(float x, float y, float z)
{
	mat3 c = MAT3_IDENTITY_MATRIX;
	c.m[0][0] = x;
	c.m[1][1] = y;
	c.m[2][2] = z;
	return c;
}

DLL_EXPORT mat3 mat3ScaleVec3(vec3 v)
{
	return mat3Scale(v.v[0], v.v[1], v.v[2]);
}

DLL_EXPORT mat3 mat3ScaleScalar(float value)
{
	return mat3Scale(value, value, value);
}

DLL_EXPORT mat2 mat2Scale(float x, float y)
{
	mat2 c = MAT2_IDENTITY_MATRIX;
	c.m[0][0] = x;
	c.m[1][1] = y;
	return c;
}

DLL_EXPORT mat2 mat2ScaleVec2(vec3 v)
{
	return mat2Scale(v.v[0], v.v[1]);
}

DLL_EXPORT mat2 mat2ScaleScalar(float value)
{
	return mat2Scale(value, value);
}

DLL_EXPORT mat4 mat4Multiply(mat4 c, mat4 d)
{
	mat4 out = MAT4_ZERO_MATRIX;
	for ( int i = 0; i < 4; ++i ) {
	    for ( int j = 0; j < 4; ++j ) {
		for ( int k = 0; k < 4; ++k ) {
		    out.m[i][j] += c.m[i][k] * d.m[k][j];
		}
	    }
	}
	return out;
}

DLL_EXPORT vec4 vec4MultiplyMat4(mat4 a, vec4 v)
{
	vec4 vout;
	float temp = 0;
	float u[4] = {0};
	u[0] = v.v[0];
	u[1] = v.v[1];
	u[2] = v.v[2];
	u[3] = v.v[3];
	for(int i = 0; i < 4; i++) {
		for(int j = 0; j < 4; j++) {
			temp += a.m[i][j]*u[j];
		}
		u[i] = temp;
		temp = 0;
	}
	vout.v[0] = u[0];
	vout.v[1] = u[1];
	vout.v[2] = u[2];
	vout.v[3] = u[3];
	return vout;
}

DLL_EXPORT vec3 vec3MultiplyMat3(mat3 a, vec3 v)
{
	vec3 vout;
	float temp = 0;
	float u[3] = {0};
	u[0] = v.v[0];
	u[1] = v.v[1];
	u[2] = v.v[2];
	for(int i = 0; i < 3; i++) {
		for(int j = 0; j < 3; j++) {
			temp += a.m[i][j]*u[j];
		}
		u[i] = temp;
		temp = 0;
	}
	vout.v[0] = u[0];
	vout.v[1] = u[1];
	vout.v[2] = u[2];
	return vout;
}

DLL_EXPORT vec2 vec2MultiplyMat2(mat2 a, vec2 v)
{
	vec2 vout;
	float temp = 0;
	float u[2] = {0};
	u[0] = v.v[0];
	u[1] = v.v[1];
	for(int i = 0; i < 2; i++) {
		for(int j = 0; j < 2; j++) {
			temp += a.m[i][j]*u[j];
		}
		u[i] = temp;
		temp = 0;
	}
	vout.v[0] = u[0];
	vout.v[1] = u[1];
	return vout;
}

DLL_EXPORT mat4 mat4Transpose(mat4 a) {
	mat4 out = MAT4_ZERO_MATRIX;
	for(int i = 0; i < 4; i++) {
		for(int j = 0; j < 4; j++) {
			out.m[j][i] = a.m[i][j];
		}
	}
	return out;
}

DLL_EXPORT mat3 mat3Transpose(mat3 a) {
	mat3 out = MAT3_ZERO_MATRIX;
	for(int i = 0; i < 3; i++) {
		for(int j = 0; j < 3; j++) {
			out.m[j][i] = a.m[i][j];
		}
	}
	return out;
}

DLL_EXPORT mat2 mat2Transpose(mat2 a) {
	mat2 out = MAT2_ZERO_MATRIX;
	for(int i = 0; i < 2; i++) {
		for(int j = 0; j < 2; j++) {
			out.m[j][i] = a.m[i][j];
		}
	}
	return out;
}

DLL_EXPORT mat4 mat4Perspective(float fovy, float aspect, float zNear, float zFar)
{
	//float half_fovy = fovy*(3.14159265358979323846 / 180.0)/2;
//
    //float top = zNear * tan(half_fovy);
    //float bottom = -top;
    //float right = top * aspect;
    //float left = -right;
	//float sx = 2 * zNear / (right - left);
    //float sy = 2 * zNear / (top - bottom);
//
    //float c2 = - (zFar + zNear) / (zFar - zNear);
    //float c1 = 2 * zNear * zFar / (zNear - zFar);
//
    //float tx = -zNear * (left + right) / (right - left);
    //float ty = -zNear * (bottom + top) / (top - bottom);
//
	//mat4 c = MAT4_ZERO_MATRIX;
    //c.m[0][0] = sx; c.m[0][1] = 0;  c.m[0][2] = 0;   c.m[0][3] = tx;
    //c.m[1][0] = 0;  c.m[1][1] = sy; c.m[1][2] = 0;   c.m[1][3] = ty;
    //c.m[2][0] = 0;  c.m[2][1] = 0;  c.m[2][2] = c2;  c.m[2][3] = c1;
    //c.m[3][0] = 0;  c.m[3][1] = 0;  c.m[3][2] = -1;  c.m[3][3] = 0;
//
	//mat4Print(c);

	mat4 c = MAT4_ZERO_MATRIX;
	float top = tan(fovy*(3.14159265358979323846 / 180.0)/2.0) * zNear;
	float right = top * aspect;
	c.m[0][0] = zNear/right;
	c.m[1][1] = zNear/top;
	c.m[2][2] = -(zFar + zNear)/(zFar - zNear);
	c.m[2][3] = -2.0*zFar*zNear/(zFar - zNear);
	c.m[3][2] = -1.0;
	c.m[3][3] = 0.0;

	return c;
}

/*
self.createFrustum = function (left, right, bottom, top, near, far) {

    var M = this.create();

    // Make sure there is no division by zero
    if (left === right || bottom === top || near === far) {
      console.log("Invalid createFrustum parameters");
      self.setIdentity(M);
    }
    if (near <= 0 || far <= 0) {
      console.log('For a perspective projection, the near and far distances must be positive');
      self.setIdentity(M);
    } else {

      var sx = 2 * near / (right - left);
      var sy = 2 * near / (top - bottom);

      var c2 = - (far + near) / (far - near);
      var c1 = 2 * near * far / (near - far);

      var tx = -near * (left + right) / (right - left);
      var ty = -near * (bottom + top) / (top - bottom);

      M[0] = sx; M[4] = 0;  M[8] = 0;    M[12] = tx;
      M[1] = 0;  M[5] = sy; M[9] = 0;    M[13] = ty;
      M[2] = 0;  M[6] = 0;  M[10] = c2;  M[14] = c1;
      M[3] = 0;  M[7] = 0;  M[11] = -1;  M[15] = 0;
    }

    return M;
  };
*/

/*
self.createPerspective = function (fovy, aspect, near, far) {

    var M;

    if (fovy <= 0 || fovy >= 180 || aspect <= 0 || near >= far || near <= 0) {
      console.log('Invalid parameters to createPerspective');
      self.setIdentity(M);
    } else {
      var half_fovy = self.toRadians(fovy) / 2;

      var top = near * Math.tan(half_fovy);
      var bottom = -top;
      var right = top * aspect;
      var left = -right;

      M = self.createFrustum(left, right, bottom, top, near, far);
    }

    return M;
  };*/

DLL_EXPORT mat4 mat4Frustum(float left, float right, float bottom, float top, float zNear, float zFar)
{
	mat4 c = MAT4_ZERO_MATRIX;
	c.m[0][0] = 2.0*zNear/(right - left);
    c.m[0][2] = (right + left)/(right - left);
    c.m[1][1] = 2.0*zNear/(top - bottom);
    c.m[1][2] = (top + bottom)/(top - bottom);
    c.m[2][2] = -(zFar + zNear)/(zFar - zNear);
    c.m[2][3] = -2.0*zFar*zNear/(zFar - zNear);
    c.m[3][2] = -1.0;
    c.m[3][3] = 0.0;
    return c;
}

DLL_EXPORT mat4 mat4Ortho(float left, float right, float bottom, float top, float zNear, float zFar )
{
    mat4 c = MAT4_ZERO_MATRIX;
    c.m[0][0] = 2.0/(right - left);
    c.m[1][1] = 2.0/(top - bottom);
    c.m[2][2] = 2.0/(zNear - zFar);
    c.m[3][3] = 1.0;
    c.m[0][3] = -(right + left)/(right - left);
    c.m[1][3] = -(top + bottom)/(top - bottom);
    c.m[2][3] = -(zFar + zNear)/(zFar - zNear);
    return c;
}

DLL_EXPORT mat4 mat4LookAt(vec3 eye, vec3 at, vec3 up)
{
	//def lookAt(self, center, target, up):
    //f = (target - center); f = f/np.linalg.norm(f)
    //s = np.cross(f, up); s = s/np.linalg.norm(s)
    //u = np.cross(s, f); u = u/np.linalg.norm(u)
	//
    //m = np.zeros((4, 4))
    //m[0, :-1] = s
    //m[1, :-1] = u
    //m[2, :-1] = -f
    //m[-1, -1] = 1.0
    
	vec3 umv = {{eye.v[0] - at.v[0], eye.v[1] - at.v[1], eye.v[2] - at.v[2]}};
	vec3 n = vec3Normalize(umv);
	vec3 u = vec3Normalize(crossProduct(up, n));
	vec3 v = vec3Normalize(crossProduct(n, u));
	mat4 c = MAT4_IDENTITY_MATRIX;
	c.m[0][0] = u.v[0];
	c.m[1][0] = u.v[1];
	c.m[2][0] = u.v[2];

	c.m[0][1] = v.v[0];
	c.m[1][1] = v.v[1];
	c.m[2][1] = v.v[2];

	c.m[0][2] = n.v[0];
	c.m[1][2] = n.v[1];
	c.m[2][2] = n.v[2];

	c.m[0][3] = 0.0f;
	c.m[1][3] = 0.0f;
	c.m[2][3] = 0.0f;
	c.m[3][3] = 1.0f;
	vec3 eyeInv = {{-eye.v[0], -eye.v[1], -eye.v[2]}};
	mat4 d = mat4Translate(eyeInv.v[0], eyeInv.v[1], eyeInv.v[2], 1.0);
	mat4 out = mat4Multiply(c, d);
	return out;
}


void vec4Print(vec4 v) {
	printf("x: %f, y: %f, z: %f, w: %f\n", v.v[0], v.v[1], v.v[2], v.v[3]);
}
void vec3Print(vec3 v) {
	printf("x: %f, y: %f, z: %f\n", v.v[0], v.v[1], v.v[2]);
}
void vec2Print(vec2 v) {
	printf("x: %f, y: %f\n", v.v[0], v.v[1]);
}

void mat4Print(mat4 m) {
	for(int i = 0; i < 16; i++) {
		printf("%f ", m.m[(int)(i/4)][i%4]);
		if(i%4 == 3)
			printf("\n");
	}
}