import ctypes
import os
import sys
import numpy as np
import time
import unittest

sys.path.insert(1, os.getcwd()+'/bin')
import geometry
import camera

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
    pass
    #g = geometry.Geometry(subdivisions=4)
    #print(g)
    #g.tetrahedron_sphere(4)
    #print(g.tetrahedron_divisions)
    #print(g.tetrahedron_points)
    #print(g.tetrahedron_normals)

class TestCamera(unittest.TestCase):
    """Test camera lib"""
    def __init__(self, *args, **kwargs):
        super(TestCamera, self).__init__(*args, **kwargs)
        self.arc_camera = camera.Camera()

    def test_camera_members(self):
        self.assertEqual(self.arc_camera.yaw, -90.0, "Testing yaw")
        self.assertEqual(self.arc_camera.pitch, 0.0, "Testing pitch")
        self.assertEqual(self.arc_camera.movement_speed, 0.1, "Testing movement_speed")
        self.assertEqual(self.arc_camera.max_speed, 2000.0, "Testing max_speed")
        self.assertEqual(self.arc_camera.mouse_sensitivity, 0.6, "Testing mouse_sensitivity")
        self.assertEqual(self.arc_camera.mouse_zoom, 1.0, "Testing mouse_zoom")

    def test_camera_getters(self):
        identity_matrix = [
            1.0, 0.0, 0.0, 0.0,
            0.0, 1.0, 0.0, 0.0,
            0.0, 0.0, 1.0, 0.0,
            0.0, 0.0, 0.0, 1.0
        ]
        zero_vector = [0.0, 0.0, 0.0]
        #self.assertEqual(self.arc_camera.view_matrix, -90.0, "Testing arc_camera.view_matrix")
        self.assertEqual(self.arc_camera.view_rotation, identity_matrix, "Testing arc_camera.view_rotation")
        self.assertEqual(self.arc_camera.view_translation, identity_matrix, "Testing arc_camera.view_translation")
        self.assertEqual(self.arc_camera.position, [0.0, 0.0, 1.0], "Testing arc_camera.position")
        self.assertEqual(self.arc_camera.rotation, zero_vector, "Testing arc_camera.rotation")

    def test_camera_movement(self):
        self.arc_camera.process_keyboard(0, 10.0)
        self.arc_camera.process_keyboard(1, 11.0)
        self.arc_camera.process_keyboard(2, 12.0)
        self.arc_camera.process_keyboard(3, 13.0)
        #print(self.arc_camera.position)

    def test_camera_mouse(self):
        self.arc_camera.process_mouse_movement(0.5, 0.5, 1)
        print(self.arc_camera.rotation)

    #def test_camera_model_position(self):
    #    self.arc_camera.camera_model_view_position(0, 0.0)
    #    #self.assertEqual(self.arc_camera.view_rotation, identity_matrix, "Testing keyboard movement")

if __name__ == "__main__":
    unittest.main()
    #test_lib()
    #mat_math = ctypes.CDLL(os.getcwd()+"/bin/matrixMath.dll")
    #print(mat_math)

    #out = mat_math.mat4RotateX(ctypes.c_double(1.0))
    #print(out)