#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include "structmember.h"
#include "sphere.h"
#include "quadCube.h"

typedef struct PyGeometryInterface {
	PyObject_HEAD
	sphere s;
	quadCube cube;
	int subdivisions;
} PyGeometryInterface;

static void Geometry_dealloc(PyGeometryInterface *self) {
    if (&self->s != NULL)
	    deallocSphere(&self->s);
	if (&self->cube != NULL)
	    deallocCube(&self->cube);
	Py_TYPE(self)->tp_free((PyObject *) self);
}

static PyObject *Geometry_new(PyTypeObject *type, PyObject *args, PyObject *kwds) {
	PyGeometryInterface *self;
	self = (PyGeometryInterface *) type->tp_alloc(type, 0);
	if (self != NULL) {
        //self->s = NULL;
		self->subdivisions = 0;
	}
	return (PyObject *) self;
}

static int Geometry_init(PyGeometryInterface *self, PyObject *args, PyObject *kwds) {
	static char *kwlist[] = {"subdivisions", NULL};

	if (!PyArg_ParseTupleAndKeywords(args, kwds, "|i", kwlist, &self->subdivisions))
		return -1;

	return 0;
}

static PyMemberDef Geometry_members[] = {
	{"tetrahedron_divisions", T_INT, offsetof(PyGeometryInterface, s.divisions), 0, "Number of tetrahedron sphere divisions"},
    {"vertexNumber", T_INT, offsetof(PyGeometryInterface, s.vertexNumber), 0, "Number of vertices"},
    {"normal_byte_size", T_INT, offsetof(PyGeometryInterface, s.nsize), 0, "Size of tetrahedron normals in bytes"},
    {"point_byte_size", T_INT, offsetof(PyGeometryInterface, s.size), 0, "Size of tetrahedron in bytes"},
	{NULL}
};

static PyObject *tetrahedron_get_points(PyGeometryInterface *self, void *closure)
{
    /*double *numero = (double *) PyMem_Malloc(sizeof(double *));
    if (NULL == numero)
        return NULL;
    *numero = 6.0;

    printf("Pointer %p and value %f\n", numero, *numero);

    PyObject * ret = PyCapsule_New(numero, "DOUBLE", NULL);
    return ret;*/
    //int N = self->s.vertexNumber*3;
	PyObject* python_val = PyList_New(self->s.vertexNumber*3);
	int index_count = 0;
	for (int i = 0; i < self->cube.vertexNumber; i++)
    {
        PyObject* point_double = Py_BuildValue("d", self->s.points[i].v[0]);
        PyList_SetItem(python_val, index_count, point_double);
		point_double = Py_BuildValue("d", self->s.points[i].v[1]);
        PyList_SetItem(python_val, index_count+1, point_double);
		point_double = Py_BuildValue("d", self->s.points[i].v[2]);
        PyList_SetItem(python_val, index_count+2, point_double);
		index_count += 3;

    }
    return python_val;
    //return Py_BuildValue("(i)", self->s.nsize);
}

static PyObject *tetrahedron_get_normals(PyGeometryInterface *self, void *closure)
{
	/*int N = self->s.vertexNumber*3;
	PyObject* python_val = PyList_New(N);
	for (int i = 0; i < N; i+=3)
    {
        PyObject* point_double = Py_BuildValue("d", self->s.points[i]);
        PyList_SetItem(python_val, i, point_double);
        point_double = Py_BuildValue("d", self->s.points[i+1]);
        PyList_SetItem(python_val, i+1, point_double);
        point_double = Py_BuildValue("d", self->s.points[i+2]);
        PyList_SetItem(python_val, i+2, point_double);
    }*/
    return Py_BuildValue("(i)", self->s.vertexNumber);
}

static PyObject *quadcube_get_points(PyGeometryInterface *self, void *closure)
{
	PyObject* python_val = PyList_New(self->cube.vertexNumber*3);
	int index_count = 0;
	for (int i = 0; i < self->cube.vertexNumber; i++)
    {
        PyObject* point_double = Py_BuildValue("d", self->cube.points[i].v[0]);
        PyList_SetItem(python_val, index_count, point_double);
		point_double = Py_BuildValue("d", self->cube.points[i].v[1]);
        PyList_SetItem(python_val, index_count+1, point_double);
		point_double = Py_BuildValue("d", self->cube.points[i].v[2]);
        PyList_SetItem(python_val, index_count+2, point_double);
		index_count += 3;

    }
    return python_val;
}

static PyObject *quadcube_get_normals(PyGeometryInterface *self, void *closure)
{
	PyObject* python_val = PyList_New(self->cube.vertexNumber*3);
	int index_count = 0;
	for (int i = 0; i < self->cube.vertexNumber; i++)
    {
        PyObject* point_double = Py_BuildValue("d", self->cube.normals[i].v[0]);
        PyList_SetItem(python_val, index_count, point_double);
		point_double = Py_BuildValue("d", self->cube.normals[i].v[1]);
        PyList_SetItem(python_val, index_count+1, point_double);
		point_double = Py_BuildValue("d", self->cube.normals[i].v[2]);
        PyList_SetItem(python_val, index_count+2, point_double);
		index_count += 3;
    }
    return python_val;
}

static PyObject *quadcube_get_tangents(PyGeometryInterface *self, void *closure)
{
	PyObject* python_val = PyList_New(self->cube.vertexNumber*3);
	int index_count = 0;
	for (int i = 0; i < self->cube.vertexNumber; i++)
    {
        PyObject* point_double = Py_BuildValue("d", self->cube.tangents[i].v[0]);
        PyList_SetItem(python_val, index_count, point_double);
		point_double = Py_BuildValue("d", self->cube.tangents[i].v[1]);
        PyList_SetItem(python_val, index_count+1, point_double);
		point_double = Py_BuildValue("d", self->cube.tangents[i].v[2]);
        PyList_SetItem(python_val, index_count+2, point_double);
		index_count += 3;
    }
    return python_val;
}


static PyGetSetDef Geometry_getsetters[] = {
    {"tetrahedron_points", (getter) tetrahedron_get_points, NULL, "Tetrahedron points", NULL},
	{"tetrahedron_normals", (getter) tetrahedron_get_normals, NULL, "Tetrahedron normals", NULL},
    {"quadcube_points", (getter) quadcube_get_points, NULL, "Quadcube points", NULL},
	{"quadcube_normals", (getter) quadcube_get_normals, NULL, "Quadcube smooth normals", NULL},
	{"quadcube_tangents", (getter) quadcube_get_tangents, NULL, "Quadcube tangents for TBN calculation", NULL},
    {NULL}  /* Sentinel */
};


static PyObject *tetrahedron_sphere(PyGeometryInterface *self, PyObject *args)
{
	const int tetrahedron_divisions;

	if (!PyArg_ParseTuple(args, "i", &tetrahedron_divisions))
		return NULL;

    tetrahedron(&self->s, tetrahedron_divisions);

	/*if (self->w.displacementdx == NULL) {
		PyErr_SetString(PyExc_AttributeError, "displacementdx");
        return NULL;
	}
	if (self->w.displacementdy == NULL) {
		PyErr_SetString(PyExc_AttributeError, "displacementdy");
        return NULL;
	}
	if (self->w.displacementdz == NULL) {
		PyErr_SetString(PyExc_AttributeError, "displacementdz");
        return NULL;
	}*/

    Py_RETURN_NONE;
}

static PyObject *quadcube_sphere(PyGeometryInterface *self, PyObject *args)
{
	const int face_divisions;

	if (!PyArg_ParseTuple(args, "i", &face_divisions))
		return NULL;

    createCube(face_divisions, &self->cube);

    Py_RETURN_NONE;
}

static PyMethodDef Geometry_methods[] = {
	{"tetrahedron_sphere", (PyCFunction) tetrahedron_sphere, METH_VARARGS, "Generate a sphere by tetrahedron subdivision."},
	{"quadcube", (PyCFunction) quadcube_sphere, METH_VARARGS, "Generate a sphere from a quadcube"},
    {NULL, NULL, 0, NULL}
};

static PyTypeObject GeometryObject =  {
	PyVarObject_HEAD_INIT(NULL, 0)
	.tp_name = "geometry.Geometry",
	.tp_doc = "Geometry",
	.tp_basicsize = sizeof(PyGeometryInterface),
	.tp_itemsize = 0,
	.tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,
	.tp_new = Geometry_new,
	.tp_init = (initproc) Geometry_init,
	.tp_dealloc = (destructor) Geometry_dealloc,
	.tp_members = Geometry_members,
	.tp_methods = Geometry_methods,
	.tp_getset = Geometry_getsetters,

};

static struct PyModuleDef GeometryModule = {
	PyModuleDef_HEAD_INIT,
	.m_name = "geometry", 				/* name of module */
    .m_doc = "Interface for creating geometric objects",		/* module documentation, may be NULL */
    .m_size = -1,						/* size of per-interpreter state of the module, or -1 if the module keeps state in global variables. */
    //.m_methods = WaterLib_FunctionsTable
};

PyMODINIT_FUNC PyInit_geometry(void)
{
	PyObject *m;
	if (PyType_Ready(&GeometryObject) < 0)
		return NULL;

	m = PyModule_Create(&GeometryModule);
	if (m == NULL)
		return NULL;

	Py_INCREF(&GeometryObject);
	if (PyModule_AddObject(m, "Geometry", (PyObject *) &GeometryObject) < 0) {
		Py_DECREF(&GeometryObject);
		Py_DECREF(m);
		return NULL;
	}
    return m;
}