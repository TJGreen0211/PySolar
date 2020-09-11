//gcc -Wall -o pytest.exe interface.c -I C:\Users\TJ\AppData\Local\Programs\Python\Python37\include -L C:\Users\TJ\AppData\Local\Programs\Python\Python37\libs -lpython37 -o pytest.dll
//gcc -c interface.c -I C:\Users\TJ\AppData\Local\Programs\Python\Python37\include
//gcc -shared interface.o -L C:\Users\TJ\AppData\Local\Programs\Python\Python37\libs -lpython37 -o hello.pyd

#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include "structmember.h"
#include "arcballCamera.h"

typedef struct PyCameraInterface {
	PyObject_HEAD
	mat4 camera_view_matrix;
	vec4 camera_model_view_position;
	ArcCamera camera;
} PyCameraInterface;

static void Waves_dealloc(PyCameraInterface *self) {
	//Py_XDECREF(self->w);
	//cleanupWaves(&self->w);
	Py_TYPE(self)->tp_free((PyObject *) self);
}

static PyObject *Camera_new(PyTypeObject *type, PyObject *args, PyObject *kwds) {
	PyCameraInterface *self;
	self = (PyCameraInterface *) type->tp_alloc(type, 0);
	if (self != NULL) {
		vec3 init_zero = {{0.0, 0.0, 0.0}};
		mat4 init_identity = mat4IdentityMatrix();
		self->camera.translation_matrix = init_identity;
		self->camera.rotation_matrix = init_identity;
		self->camera.rotation = init_zero;
		self->camera.position = init_zero;
		self->camera.up = init_zero;
		self->camera.front = init_zero;
		self->camera.right = init_zero;
		self->camera.position = init_zero;

		self->camera_view_matrix = init_identity;
		vec4 init_zero4 = {{0.0, 0.0, 0.0, 1.0}};
		self->camera_model_view_position = init_zero4;
	}
	return (PyObject *) self;
}

static int Camera_init(PyCameraInterface *self, PyObject *args, PyObject *kwds) {
	//static char *kwlist[] = {"dimension", NULL};

	vec3 init_zero = {{0.0, 0.0, 0.0}};
	vec3 init_right = {{1.0, 0.0, 0.0}};
	vec3 init_front = {{0.0, 1.0, 0.0}};
	vec3 init_up = {{0.0, 0.0, 1.0}};
	vec3 init_position = {{0.0, 0.0, 0.0}};
	mat4 init_identity = mat4IdentityMatrix();
	self->camera.translation_matrix = init_identity;
	self->camera.rotation_matrix = init_identity;
	self->camera.rotation = init_up;
	self->camera.position = init_zero;
	self->camera.up = init_up;
	self->camera.front = init_front;
	self->camera.right = init_right;
	self->camera.position = init_position;
	self->camera_view_matrix = init_identity;
	vec4 init_zero4 = {{0.0, 0.0, 0.0, 1.0}};
	self->camera_model_view_position = init_zero4;

	self->camera.yaw = -90.0;
	self->camera.pitch =0.0;
	self->camera.movement_speed = 1.1;
	self->camera.max_speed = 2000.0;
	self->camera.mouse_sensitivity = 0.6;
	self->camera.mouse_zoom = 1.0;
	
	return 0;
}

static PyMemberDef Camera_members[] = {
	{"yaw", T_DOUBLE, offsetof(PyCameraInterface, camera.yaw), 0, "Yaw"},
	{"pitch", T_DOUBLE, offsetof(PyCameraInterface, camera.pitch), 0, "Pitch"},
	{"movement_speed", T_DOUBLE, offsetof(PyCameraInterface, camera.movement_speed), 0, "Movement speed"},
	{"max_speed", T_DOUBLE, offsetof(PyCameraInterface, camera.max_speed), 0, "Max speed"},
	{"mouse_sensitivity", T_DOUBLE, offsetof(PyCameraInterface, camera.mouse_sensitivity), 0, "Mouse sensitivity"},
	{"mouse_zoom", T_DOUBLE, offsetof(PyCameraInterface, camera.mouse_zoom), 0, "Mouse zoom"},
	{NULL}
};



static PyObject *Camera_getViewMatrix(PyCameraInterface *self, void *closure)
{
	mat4 view_matrix = getViewMatrix(&self->camera);
	int num_elements = sizeof(view_matrix)/sizeof(view_matrix.m[0][0]);
	PyObject* python_val = PyList_New(num_elements);
	for (int i = 0; i < num_elements; i++)
    {
        PyObject* point_double = Py_BuildValue("d", view_matrix.m[(int)(i/4)][i%4]);
        PyList_SetItem(python_val, i, point_double);
    }
	//mat4Print(view_matrix);
    return python_val;
}

static PyObject *Camera_getViewRotation(PyCameraInterface *self, void *closure)
{
	int num_elements = sizeof(self->camera.rotation_matrix)/sizeof(self->camera.rotation_matrix.m[0][0]);
	PyObject* python_val = PyList_New(num_elements);
	for (int i = 0; i < num_elements; i++)
    {
        PyObject* point_double = Py_BuildValue("d", self->camera.rotation_matrix.m[(int)(i/4)][i%4]);
        PyList_SetItem(python_val, i, point_double);
    }
    return python_val;
}

static PyObject *Camera_getViewTranslation(PyCameraInterface *self, void *closure)
{
	int num_elements = sizeof(self->camera.translation_matrix)/sizeof(self->camera.translation_matrix.m[0][0]);
	PyObject* python_val = PyList_New(num_elements);
	for (int i = 0; i < num_elements; i++)
    {
        PyObject* point_double = Py_BuildValue("d", self->camera.translation_matrix.m[(int)(i/4)][i%4]);
        PyList_SetItem(python_val, i, point_double);
    }
    return python_val;
}

static PyObject *Camera_getRotation(PyCameraInterface *self, void *closure)
{
	int num_elements = sizeof(self->camera.rotation)/sizeof(self->camera.rotation.v[0]);
	PyObject* python_val = PyList_New(num_elements);
	for (int i = 0; i < num_elements; i++)
    {
        PyObject* point_double = Py_BuildValue("d", self->camera.rotation.v[i]);
        PyList_SetItem(python_val, i, point_double);
    }
    return python_val;
}

static PyObject *Camera_getPosition(PyCameraInterface *self, void *closure)
{
	int num_elements = sizeof(self->camera.position)/sizeof(self->camera.position.v[0]);
	PyObject* python_val = PyList_New(num_elements);
	for (int i = 0; i < num_elements; i++)
    {
        PyObject* point_double = Py_BuildValue("d", self->camera.position.v[i]);
        PyList_SetItem(python_val, i, point_double);
    }
    return python_val;
}

static PyGetSetDef Camera_getsetters[] = {
	{"view_matrix", (getter) Camera_getViewMatrix, NULL, "Camera view matrix", NULL},
	{"view_rotation", (getter) Camera_getViewRotation, NULL, "Camera rotation matrix", NULL},
	{"view_translation", (getter) Camera_getViewTranslation, NULL, "Camera translation matrix", NULL},
    {"position", (getter) Camera_getPosition, NULL, "Camera position vector", NULL},
	{"rotation", (getter) Camera_getRotation, NULL, "Camera rotation vector", NULL},
    {NULL}  /* Sentinel */
};

static PyObject *get_perspective_matrix(PyCameraInterface *self, PyObject *args)
{
	const double fov;
	const double aspect;
	const double z_near;
	const double z_far;

	if (!PyArg_ParseTuple(args, "dddd", &fov, &aspect, &z_near, &z_far))
		return NULL;

	mat4 perspective = mat4Perspective( fov, aspect, z_near, z_far);
	int num_elements = sizeof(perspective)/sizeof(perspective.m[0][0]);
	PyObject* python_val = PyList_New(num_elements);
	for (int i = 0; i < num_elements; i++)
    {
        PyObject* point_double = Py_BuildValue("d", perspective.m[(int)(i/4)][i%4]);
        PyList_SetItem(python_val, i, point_double);
    }
    return python_val;
}

//static PyObject *process_keyboard(PyCameraInterface *self, PyObject *Py_UNUSED(ignored))
static PyObject *get_camera_position(PyCameraInterface *self, PyObject *args)
{
	PyObject *float_list;
    int pr_length;
    double *pr;

	if (!PyArg_ParseTuple(args, "O", &float_list))
        return NULL;

	pr_length = PyObject_Length(float_list);
    if (pr_length < 0)
        return NULL;
    pr = (double *) malloc(sizeof(double *) * pr_length);
    if (pr == NULL)
        return NULL;
    for (int index = 0; index < pr_length; index++) {
        PyObject *item;
        item = PyList_GetItem(float_list, index);
        if (!PyFloat_Check(item))
            pr[index] = 0.0;
        pr[index] = PyFloat_AsDouble(item);
    }
	// TODO: this
    //return Py_BuildValue("i", _asdf(pr, pr_length));

	mat4 position_matrix = {
		{{pr[0], pr[1], pr[2], pr[3]},
		{pr[4], pr[5], pr[6], pr[7]},
		{pr[8], pr[9], pr[10], pr[11]},
		{pr[12], pr[13], pr[14], pr[15]}}
	};

	vec4 ret = getCameraPosition(self->camera, position_matrix);
	//printf("%f, %f, %f, %f\n", ret.v[0], ret.v[1], ret.v[2], ret.v[3]);

    return  Py_BuildValue("[d,d,d,d]", ret.v[0], ret.v[1], ret.v[2], ret.v[3]);
}

//static PyObject *process_keyboard(PyCameraInterface *self, PyObject *Py_UNUSED(ignored))
static PyObject *process_keyboard(PyCameraInterface *self, PyObject *args)
{
	const double time;
	const int direction;

	if (!PyArg_ParseTuple(args, "id", &direction, &time))
		return NULL;

	processKeyboard(&self->camera, direction, time);

    Py_RETURN_NONE;
}

static PyObject *process_mouse_movement(PyCameraInterface *self, PyObject *args)
{
	const double xpos;
	const double ypos;
	const int reset_flag;

	if (!PyArg_ParseTuple(args, "ddi", &xpos, &ypos, &reset_flag))
		return NULL;

	processMouseMovement(&self->camera, xpos, ypos, reset_flag);

    Py_RETURN_NONE;
}

static PyObject *process_mouse_scroll(PyCameraInterface *self, PyObject *args)
{
	const double yoffset;

	if (!PyArg_ParseTuple(args, "d", &yoffset))
		return NULL;

	processMouseScroll(&self->camera, yoffset);

    Py_RETURN_NONE;
}

static PyMethodDef Camera_methods[] = {
	{ "process_keyboard", (PyCFunction) process_keyboard, METH_VARARGS, "Process keybard input" },
	{ "process_mouse_movement", (PyCFunction) process_mouse_movement, METH_VARARGS, "Process mouse input and movement" },
	{ "process_mouse_scroll", (PyCFunction) process_mouse_scroll, METH_VARARGS, "Process mouse scroll" },
	{ "camera_model_view_position", (PyCFunction) get_camera_position, METH_VARARGS, "Get camera position relative to the model" },
	{ "camera_perspective_matrix", (PyCFunction) get_perspective_matrix, METH_VARARGS, "Get perspective matrix" },
	{NULL, NULL, 0, NULL}		// Sentinal
};

static PyTypeObject CameraObject =  {
	PyVarObject_HEAD_INIT(NULL, 0)
	.tp_name = "camera.Camera",
	.tp_doc = "Camera",
	.tp_basicsize = sizeof(PyCameraInterface),
	.tp_itemsize = 0,
	.tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,
	.tp_new = Camera_new,
	.tp_init = (initproc) Camera_init,
	.tp_dealloc = (destructor) Waves_dealloc,
	.tp_members = Camera_members,
	.tp_methods = Camera_methods,
	.tp_getset = Camera_getsetters,

};

static struct PyModuleDef cameraModule = {
	PyModuleDef_HEAD_INIT,
	.m_name = "arcCamera", 				/* name of module */
    .m_doc = "Module for creating an arcball camera",		/* module documentation, may be NULL */
    .m_size = -1,						/* size of per-interpreter state of the module, or -1 if the module keeps state in global variables. */
};

PyMODINIT_FUNC PyInit_camera(void)
{
	PyObject *m;
	if (PyType_Ready(&CameraObject) < 0)
		return NULL;

	m = PyModule_Create(&cameraModule);
	if (m == NULL)
		return NULL;

	Py_INCREF(&CameraObject);
	if (PyModule_AddObject(m, "Camera", (PyObject *) &CameraObject) < 0) {
		Py_DECREF(&CameraObject);
		Py_DECREF(m);
		return NULL;
	}
    return m;
}