//gcc -Wall -o pytest.exe interface.c -I C:\Users\TJ\AppData\Local\Programs\Python\Python37\include -L C:\Users\TJ\AppData\Local\Programs\Python\Python37\libs -lpython37 -o pytest.dll
//gcc -c interface.c -I C:\Users\TJ\AppData\Local\Programs\Python\Python37\include
//gcc -shared interface.o -L C:\Users\TJ\AppData\Local\Programs\Python\Python37\libs -lpython37 -o hello.pyd

#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include "structmember.h"
#include "waves.h"

typedef struct PyWavesInterface {
	PyObject_HEAD
	waves w;
	int wavePatchDimension;
} PyWavesInterface;

static void Waves_dealloc(PyWavesInterface *self) {
	//Py_XDECREF(self->w);
	cleanupWaves(&self->w);
	Py_TYPE(self)->tp_free((PyObject *) self);
}

static PyObject *Waves_new(PyTypeObject *type, PyObject *args, PyObject *kwds) {
	PyWavesInterface *self;
	self = (PyWavesInterface *) type->tp_alloc(type, 0);
	if (self != NULL) {
		/*initializeWaves(self->w, 16);
		self->w =
		if (self->w == NULL) {
			Py_DECREF(self);
			return NULL;
		}*/
		self->wavePatchDimension = 0;
	}
	return (PyObject *) self;
}

static int Waves_init(PyWavesInterface *self, PyObject *args, PyObject *kwds) {
	static char *kwlist[] = {"dimension", NULL};
	//PyObject *w = NULL;

	if (!PyArg_ParseTupleAndKeywords(args, kwds, "|i", kwlist, &self->wavePatchDimension))
		return -1;

	initializeWaves(&self->w, self->wavePatchDimension);

	return 0;
}

static PyMemberDef Waves_members[] = {
	{"dimension", T_INT, offsetof(PyWavesInterface, w.dimension), 0, "Wave dimension"},
	//{"dx", T_OBJECT_EX, offsetof(PyWavesInterface, w.displacementdx), 0, "Displacement map X"},
	//{"dy", T_OBJECT_EX, offsetof(PyWavesInterface, w.displacementdy), 0, "Displacement map Y"},
	//{"dz", T_OBJECT_EX, offsetof(PyWavesInterface, w.displacementdz), 0, "Displacement map Z"},
	{NULL}
};


static PyObject *Waves_getdx(PyWavesInterface *self, void *closure)
{
	//return Py_BuildValue("*d", self->w.displacementdx);

	int N = self->w.dimension*(self->w.dimension)*4;
	PyObject* python_val = PyList_New(N);
	for (int i = 0; i < N; i++)
    {
        PyObject* dx_float = Py_BuildValue("d", self->w.displacementdx[i]);
        PyList_SetItem(python_val, i, dx_float);
    }
    return python_val;
    //Py_INCREF(self->w.displacementdx);
    //return self->w.displacementdx;
}

static PyObject *Waves_getdy(PyWavesInterface *self, void *closure)
{
    //Py_INCREF(self->w.displacementdy);
	//return Py_BuildValue("*d", self->w.displacementdy);
    //return self->w.displacementdy;

	int N = self->w.dimension*(self->w.dimension)*4;
	PyObject* python_val = PyList_New(N);
	for (int i = 0; i < N; i++)
    {
        PyObject* dy_float = Py_BuildValue("d", self->w.displacementdy[i]);
        PyList_SetItem(python_val, i, dy_float);
		//PyList_Append(list, PyFloat_FromDouble(element))
    }
    return python_val;
}

static PyObject *Waves_getdz(PyWavesInterface *self, void *closure)
{
	//return Py_BuildValue("*d", self->w.displacementdz);
    //Py_INCREF(self->w.displacementdz);
    //return self->w.displacementdz;

	int N = self->w.dimension*(self->w.dimension)*4;
	PyObject* python_val = PyList_New(N);
	for (int i = 0; i < N; i++)
    {
        PyObject* dz_float = Py_BuildValue("d", self->w.displacementdz[i]);
        PyList_SetItem(python_val, i, dz_float);
    }
    return python_val;
}

static PyGetSetDef Waves_getsetters[] = {
    {"dx", (getter) Waves_getdx, NULL, "Displacement map X", NULL},
	{"dy", (getter) Waves_getdy, NULL, "Displacement map Y", NULL},
	{"dz", (getter) Waves_getdz, NULL, "Displacement map Z", NULL},
    {NULL}  /* Sentinel */
};


//static PyObject *generate_waves(PyWavesInterface *self, PyObject *Py_UNUSED(ignored))
static PyObject *generate_waves(PyWavesInterface *self, PyObject *args)
{
	const double time;

	if (!PyArg_ParseTuple(args, "d", &time))
		return NULL;

	generateWaves(&self->w, time);

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

static PyMethodDef Waves_methods[] = {
	{
		"generate_waves", 	// Name exposed to Python
		(PyCFunction) generate_waves, 	// C wrapper function
		METH_VARARGS, 		// Recieved variable args
		"Generate the wave patches."	// Documentation
	}, {NULL, NULL, 0, NULL}		// Sentinal
};

static PyTypeObject WavesObject =  {
	PyVarObject_HEAD_INIT(NULL, 0)
	.tp_name = "waves.Waves",
	.tp_doc = "Wave patches",
	.tp_basicsize = sizeof(PyWavesInterface),
	.tp_itemsize = 0,
	.tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,
	.tp_new = Waves_new,
	.tp_init = (initproc) Waves_init,
	.tp_dealloc = (destructor) Waves_dealloc,
	.tp_members = Waves_members,
	.tp_methods = Waves_methods,
	.tp_getset = Waves_getsetters,

};

static struct PyModuleDef wavesModule = {
	PyModuleDef_HEAD_INIT,
	.m_name = "waves", 				/* name of module */
    .m_doc = "Module for creating wave patches",		/* module documentation, may be NULL */
    .m_size = -1,						/* size of per-interpreter state of the module, or -1 if the module keeps state in global variables. */
    //.m_methods = WaterLib_FunctionsTable
};

PyMODINIT_FUNC PyInit_waves(void)
{
	PyObject *m;
	if (PyType_Ready(&WavesObject) < 0)
		return NULL;

	m = PyModule_Create(&wavesModule);
	if (m == NULL)
		return NULL;

	Py_INCREF(&WavesObject);
	if (PyModule_AddObject(m, "Waves", (PyObject *) &WavesObject) < 0) {
		Py_DECREF(&WavesObject);
		Py_DECREF(m);
		return NULL;
	}
    return m;
	//return PyModule_Create(&wavesModule);
}