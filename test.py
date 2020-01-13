import ctypes
import os
import sys
import numpy as np
import time

sys.path.insert(1, os.getcwd()+'/bin')
import geometry

def test():
    #numbers = [1, 2, -3, -4, -5, 6]
    #test_lib = ctypes.CDLL("C:/Users/TJ/Documents/programming/python/PyGL/test.dll")
    #print(test_lib)
    #test_lib.test_function.argtypes = (ctypes.c_int, ctypes.POINTER(ctypes.c_int))
    #test_lib.test_function.restype = ctypes.POINTER(ctypes.c_int * len(numbers))
    #num_numbers = len(numbers)
    #array_type = ctypes.c_int * num_numbers
    #results = test_lib.test_function(ctypes.c_int(num_numbers), array_type(*numbers))

    #print_msg_code = test_lib.print_msg();
    #print(print_msg_code)
    #print([i for i in results.contents])
    #
    #a = np.array([
    #	[1.+0.j, 0.+0.j, 0.+0.j, 0.+0.j],
    #	[2.+0.j, 0.+0.j, 0.+0.j, 0.+0.j],
    #	[3.+0.j, 4.+0.j, 5.+0.j, 6.+0.j],
    #	[4.+0.j, 5.+0.j, 6.+0.j, 7.+0.j]
    #])

    #a = np.array([
    #	[2.687500+0.000000j], [-0.062500+-0.250000j], [-0.062500+0.000000j], [-0.062500+0.250000j],
    #	[-1.062500+-1.250000j], [0.062500+0.375000j], [0.187500+0.250000j], [0.312500+0.125000j],
    #	[-0.312500+0.000000j], [-0.062500+0.000000j], [-0.062500+0.000000j], [-0.062500+0.000000j],
    #	[-1.062500+1.250000j], [0.312500+-0.125000j], [0.187500+-0.250000j], [0.062500+-0.375000j]
    #])
    #print(np.fft.ifft2(a))
    #print(np.fft.fft2(a))
    #
    #a = np.array

    #test_lib = ctypes.cdll.LoadLibrary('C:/Users/TJ/Documents/programming/python/PyGL/test.dll')
    #test_lib = ctypes.CDLL("C:/Users/TJ/Documents/programming/python/PyGL/test.dll")
    #print(test_lib)

    dim = 128
    waves_lib = ctypes.CDLL("C:/Users/TJ/Documents/programming/python/PyGL/waves.dll")
    print(waves_lib)
    #waves_lib.initializeWaveInterface.argtypes = (ctypes.c_int, ctypes.POINTER(ctypes.c_int))
    waves_lib.initializeWaveInterface(ctypes.c_int(dim))
    #waves_lib.generateWavePatches.argtypes = (ctypes.c_double)
    waves_lib.generateWavePatches(ctypes.c_double(1.0))

    waves_lib.getDisplacementdx.restype = ctypes.POINTER(ctypes.c_double * (dim*dim*4))
    dx_texture = waves_lib.getDisplacementdx()
    print([i for i in dx_texture.contents])

def test_lib():
    g = geometry.Geometry(subdivisions=4)
    print(g)
    g.tetrahedron_sphere(4)
    print(g.tetrahedron_divisions)
    print(g.tetrahedron_points)
    #print(g.tetrahedron_normals)

if __name__ == "__main__":
    test_lib()
    #mat_math = ctypes.CDLL(os.getcwd()+"/bin/matrixMath.dll")
    #print(mat_math)

    #out = mat_math.mat4RotateX(ctypes.c_double(1.0))
    #print(out)