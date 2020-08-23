"""Main"""
# pylint: disable=C0111
# pylint: disable=C0301
import os
import sys
import math
import ctypes
import time
import numpy as np

import kivy

#pylint: disable=wrong-import-position
kivy.require('1.1.1')
from kivy.app import App
from kivy.clock import Clock
from kivy.core.window import Window
from kivy.core.image import Image
from kivy.uix.widget import Widget
from kivy.uix.floatlayout import FloatLayout
from kivy.resources import resource_find
#from kivy.graphics.transformation import Matrix
from kivy.graphics import Fbo
from kivy.graphics import Rectangle
from kivy.graphics import opengl

from shader import Shader

sys.path.insert(1, os.getcwd()+'/bin')

import waves
import geometry
import camera

#pylint: enable=wrong-import-position

with open("shaders/shader.frag", 'r') as f:
    FRAGMENT_CODE = f.read().encode()



DATA = np.array([-0.5, -0.5, 0.5,
                 0.5, -0.5, 0.5,
                 0.5, 0.5, 0.5,
                 -0.5, 0.5, 0.5], dtype=np.float32)

INDEX = np.array([0, 1, 2, 2, 3, 0], dtype=np.uint32)


"""Application"""
class Application(Widget):
    def __init__(self, **kwargs):
        super(Application, self).__init__(**kwargs)
        self.angle = 0
        self.setup_kvfbo()
        self.setup_glfbo()
        self.wave_patch = waves.Waves(dimension=128)
        self._keyboard = Window.request_keyboard(self._keyboard_closed, self)
        self._keyboard.bind(on_key_down=self._on_keyboard_down)

        self.arcball_camera = camera.Camera()
        self.perspective_matrix = self.arcball_camera.camera_perspective_matrix(90.0, 640.0/480.0, 0.1, 500.0)
        self.frame_count = 0

        #self.application_start_time = time.time()

        Clock.schedule_interval(self.update_glsl, 1.0 / 60.0)

    def _keyboard_closed(self):
        self._keyboard.unbind(on_key_down=self._on_keyboard_down)
        self._keyboard = None

    def _on_keyboard_down(self, keyboard, keycode, text, modifiers):
        #delta_time = time.time() - self.application_start_time
        if keycode[1] == 'w':
            self.arcball_camera.process_keyboard(0, 0.1)
        elif keycode[1] == 's':
            self.arcball_camera.process_keyboard(1, 0.1)
        elif keycode[1] == 'a':
            self.arcball_camera.process_keyboard(2, 0.1)
        elif keycode[1] == 'd':
            self.arcball_camera.process_keyboard(3, 0.1)
        return True

    def on_touch_down(self, touch):
        if touch.is_mouse_scrolling:
            if touch.button == 'scrolldown':
                self.arcball_camera.process_mouse_scroll(-0.01)
            elif touch.button == 'scrollup':
                self.arcball_camera.process_mouse_scroll(0.01)


    def on_touch_move(self, touch):
        #print("Touch move", touch)
        screen_position = list(touch.spos)
        self.arcball_camera.process_mouse_movement(screen_position[0], screen_position[1], 0)

    #def on_touch_up(self, touch):
    #    print("Touch Release", touch)

    def update_glsl(self, *largs):
        self.angle += 0.01
        #self.waves_lib.generateWavePatches(ctypes.c_double(self.angle))
        #self.waves_lib.getDisplacementdx.restype = ctypes.POINTER(ctypes.c_double * (self.dim2*4))
        #self.waves_lib.getDisplacementdy.restype = ctypes.POINTER(ctypes.c_double * (self.dim2*4))
        #self.waves_lib.getDisplacementdz.restype = ctypes.POINTER(ctypes.c_double * (self.dim2*4))

        self.wave_patch.generate_waves(self.angle)
        #if 0:
        #    self.draw_fbo(self.kvfboid)
        #else:
        self.draw_fbo(self.glfboid)
        self.blit_fbo()
        self.canvas.ask_update()

    def setup_kvfbo(self):
        self.kvfbo = Fbo(with_depthbuffer=True, size=Window.size, compute_normal_mat=True)
        self.canvas.add(self.kvfbo)
        self.canvas.add(Rectangle(size=Window.size, texture=self.kvfbo.texture))
        self.kvfbo.bind()
        (self.kvfboid,) = opengl.glGetIntegerv(opengl.GL_FRAMEBUFFER_BINDING)
        self.kvfbo.release()

    def setup_glfbo(self):
        self.light_shader = Shader()

        vertex = self.light_shader.create_shader("shaders/shader.vert", opengl.GL_VERTEX_SHADER)
        fragment = self.light_shader.create_shader("shaders/shader.frag", opengl.GL_FRAGMENT_SHADER)

        opengl.glAttachShader(self.light_shader.get_program(), vertex)
        opengl.glAttachShader(self.light_shader.get_program(), fragment)
        opengl.glLinkProgram(self.light_shader.get_program())
        opengl.glDetachShader(self.light_shader.get_program(), vertex)
        opengl.glDetachShader(self.light_shader.get_program(), fragment)

        self.program = self.light_shader.get_program()
        
        opengl.glUseProgram(self.program)
        
        self.w, self.h = Window.width, Window.height
        opengl.glEnable(opengl.GL_TEXTURE_2D)
        (self.glfboid,) = opengl.glGenFramebuffers(1)
        (self.gltexid,) = opengl.glGenTextures(1)
        opengl.glBindFramebuffer(opengl.GL_FRAMEBUFFER, self.glfboid)
        opengl.glBindTexture(opengl.GL_TEXTURE_2D, self.gltexid)

        opengl.glTexImage2D(opengl.GL_TEXTURE_2D, 0, opengl.GL_RGBA, self.w, self.h, 0, opengl.GL_RGBA, opengl.GL_UNSIGNED_BYTE, (np.ones(self.w*self.h*4, np.uint8)*128).tobytes())
        opengl.glFramebufferTexture2D(opengl.GL_FRAMEBUFFER, opengl.GL_COLOR_ATTACHMENT0, opengl.GL_TEXTURE_2D, self.gltexid, 0)
        opengl.glBindFramebuffer(opengl.GL_FRAMEBUFFER, 0)
        opengl.glBindTexture(opengl.GL_TEXTURE_2D, 0)

        """g = geometry.Geometry(subdivisions=4)
        g.tetrahedron_sphere(4)
        #print(g.tetrahedron_divisions)
        #print(g.tetrahedron_points)
        (self.sphereVBO,) = opengl.glGenBuffers(1)
        opengl.glBindBuffer(opengl.GL_ARRAY_BUFFER, self.sphereVB)
        opengl.glBufferData(opengl.GL_ARRAY_BUFFER, g.point_byte_size + g.normal_byte_size, None, opengl.GL_STATIC_DRAW)
        opengl.glBufferSubData(opengl.GL_ARRAY_BUFFER, 0,	g.point_byte_size, bytearray(g.tetrahedron_points))
        opengl.glBufferSubData(opengl.GL_ARRAY_BUFFER, g.point_byte_size, g.normal_byte_size, g.tetrahedron_normals)"""

        self.g = geometry.Geometry(subdivisions=1)
        self.g.quadcube(10)
        self.sphere = np.array(self.g.quadcube_points, dtype=np.float32)
        self.sphere_normals = np.array(self.g.quadcube_normals, dtype=np.float32)

        (self.vbo,) = opengl.glGenBuffers(1)
        opengl.glBindBuffer(opengl.GL_ARRAY_BUFFER, self.vbo)
        opengl.glBufferData(opengl.GL_ARRAY_BUFFER, self.sphere.nbytes, self.sphere.tobytes(), opengl.GL_STATIC_DRAW)
        #(self.ibo,) = opengl.glGenBuffers(1)
        #opengl.glBindBuffer(opengl.GL_ELEMENT_ARRAY_BUFFER, self.ibo)
        #opengl.glBufferData(opengl.GL_ELEMENT_ARRAY_BUFFER, INDEX.nbytes, INDEX.tobytes(), opengl.GL_STATIC_DRAW)

    def blit_fbo(self):
        opengl.glBindFramebuffer(opengl.GL_FRAMEBUFFER, self.glfboid)
        pixels = opengl.glReadPixels(0, 0, self.w, self.h, opengl.GL_RGBA, opengl.GL_UNSIGNED_BYTE)
        opengl.glBindFramebuffer(opengl.GL_FRAMEBUFFER, self.kvfboid)
        opengl.glTexSubImage2D(opengl.GL_TEXTURE_2D, 0, 0, 0, self.w, self.h, opengl.GL_RGBA, opengl.GL_UNSIGNED_BYTE, pixels)
        opengl.glBindFramebuffer(opengl.GL_FRAMEBUFFER, 0)

    def draw_fbo(self, targetfbo):
        model = np.matrix([[1, 0, 0, 0],
                         [0, 1, 0, 0],
                         [0, 0, 1, 2],
                         [0, 0, 0, 1]], dtype=np.float32)
                         

        perspective_arr = np.array(self.perspective_matrix, dtype=np.float32).reshape(4, 4)
        view_arr = np.array(self.arcball_camera.view_matrix, dtype=np.float32).reshape(4, 4)
        #model_arr = np.array(self.arcball_camera.view_translation, dtype=np.float32).reshape(4, 4)
        model_arr = np.array(model, dtype=np.float32).reshape(4, 4)
        #model_arr = np.identity(4, dtype=np.float32)

        self.frame_count += 1
        if self.frame_count %60 == 0:
        #    print("\n FRAME: {}".format(self.frame_count))
        #    print(self.arcball_camera.view_matrix)
        #    print(self.arcball_camera.view_rotation)
        #    print(self.arcball_camera.view_translation)
            print(self.arcball_camera.position)
        #    print(self.arcball_camera.rotation)
        #    print(self.arcball_camera.yaw)
        #    print(self.arcball_camera.pitch)
        #    print(self.arcball_camera.movement_speed)
        #    print(self.arcball_camera.max_speed)
        #    print(self.arcball_camera.mouse_sensitivity)
        #    print(self.arcball_camera.mouse_zoom)

        opengl.glBindFramebuffer(opengl.GL_FRAMEBUFFER, targetfbo)
        opengl.glClear(opengl.GL_COLOR_BUFFER_BIT)

        opengl.glBindBuffer(opengl.GL_ARRAY_BUFFER, self.vbo)
        #opengl.glBindBuffer(opengl.GL_ELEMENT_ARRAY_BUFFER, self.ibo)

        opengl.glUseProgram(self.program)
        u_loc = opengl.glGetUniformLocation(self.program, b"projection")
        opengl.glUniformMatrix4fv(u_loc, 1, False, np.array(perspective_arr).flatten().tobytes())

        view_loc = opengl.glGetUniformLocation(self.program, b"view")
        opengl.glUniformMatrix4fv(view_loc, 1, False, view_arr.flatten().tobytes())

        model_loc = opengl.glGetUniformLocation(self.program, b"model")
        opengl.glUniformMatrix4fv(model_loc, 1, False, model_arr.flatten().tobytes())

        #a_loc = opengl.glGetAttribLocation(self.program, b"a_position")
        #opengl.glUniformMatrix4fv(opengl.glGetUniformLocation(self.program, b"model"), 1, True, np.array(model_arr).flatten().tobytes())
        #perspective = opengl.glGetAttribLocation(self.program, b"perspective")
        #opengl.glUniformMatrix4fv(opengl.glGetUniformLocation(self.program, b"perspective"), 1, True, np.array(perspective_arr).flatten().tobytes())

        opengl.glEnableVertexAttribArray(0)

        #glUniformMatrix4fv(glGetUniformLocation( shader, "projection" ), 1, GL_FALSE, &p.m[0][0]);
        #print("DATA POINTS", 3*sys.getsizeof(bytearray(f)[0]))
        #print("DATA POINTS", bytearray(f))
        opengl.glVertexAttribPointer(0, 3, opengl.GL_FLOAT, False, 12, 0)
        #opengl.glVertexAttribPointer(0, 3, opengl.GL_FLOAT, opengl.GL_FALSE, 3*sys.getsizeof(data.points[0]), ctypes.c_void_p(0))

        opengl.glViewport(0, 0, self.w, self.h)
        opengl.glEnable(opengl.GL_CULL_FACE)
        opengl.glCullFace(opengl.GL_BACK);  

        #opengl.glDrawElements(opengl.GL_TRIANGLES, 6, opengl.GL_UNSIGNED_INT, 0)
        opengl.glDrawArrays(opengl.GL_TRIANGLES, 0, len(self.sphere))
        opengl.glBindFramebuffer(opengl.GL_FRAMEBUFFER, 0)

    def update(self, dt):
        #dx_texture = glGenTextures(1)
        #dy_texture = glGenTextures(1)
        #dz_texture = glGenTextures(1)

        #glViewport(0, 0, width, height)
        opengl.glEnable(opengl.GL_DEPTH_TEST)
        #opengl.glDisable(opengl.GL_CULL_FACE)
        opengl.glClearColor(0.5, 0.5, 0.5, 1.0)
        opengl.glClear(opengl.GL_COLOR_BUFFER_BIT | opengl.GL_DEPTH_BUFFER_BIT)

        #bindTextureAttachment(dx_texture, dx_texture_data, dim, dim)
        #bindTextureAttachment(dy_texture, dy_texture_data, dim, dim)
        #bindTextureAttachment(dz_texture, dz_texture_data, dim, dim)

        #print(dy_texture_data)

class MainApp(App):
    cwd = os.getcwd()
    os.chdir(cwd)

    def build(self):
        return Application()

def createBuffer(data):
    vao = opengl.glGenVertexArrays(1)
    opengl.glBindVertexArray(vao)
    vbo = opengl.glGenBuffers(1)
    opengl.glBindBuffer(opengl.GL_ARRAY_BUFFER, vbo)

    #opengl.glBufferData(opengl.GL_ARRAY_BUFFER, opengl.ArrayDatatype.arrayByteCount(vertices)+opengl.ArrayDatatype.arrayByteCount(normals)+opengl.ArrayDatatype.arrayByteCount(texCoords), None, opengl.GL_STATIC_DRAW)
    opengl.glBufferData(opengl.GL_ARRAY_BUFFER, data.size, None, opengl.GL_STATIC_DRAW)
    opengl.glBufferSubData(opengl.GL_ARRAY_BUFFER, 0, data.size, bytearray(data.points))
    #opengl.glBufferSubData(opengl.GL_ARRAY_BUFFER, opengl.ArrayDatatype.arrayByteCount(vertices), opengl.ArrayDatatype.arrayByteCount(normals), normals)
    #opengl.glBufferSubData(opengl.GL_ARRAY_BUFFER, opengl.ArrayDatatype.arrayByteCount(vertices)+opengl.ArrayDatatype.arrayByteCount(normals), opengl.ArrayDatatype.arrayByteCount(texCoords), texCoords)

    opengl.glVertexAttribPointer(0, 3, opengl.GL_FLOAT, opengl.GL_FALSE, 3*sys.getsizeof(data.points[0]), 0)
    opengl.glEnableVertexAttribArray(0)
    #opengl.glVertexAttribPointer(1, 3, opengl.GL_FLOAT, opengl.GL_FALSE, opengl.ArrayDatatype.ArrayByteCount(normals[0]), ctypes.c_void_p(vertices.itemsize))
    #opengl.glEnableVertexAttribArray(1)
    #opengl.glVertexAttribPointer(2, 2, opengl.GL_FLOAT, opengl.GL_FALSE, opengl.ArrayDatatype.ArrayByteCount(texCoords[0]), ctypes.c_void_p(vertices.itemsize+texCoords.itemsize))
    #opengl.glEnableVertexAttribArray(2)

    opengl.glBindBuffer(opengl.GL_ARRAY_BUFFER, 0)
    opengl.glBindVertexArray(0)

    return vao

"""def drawObject(shader, vao, position, texture, vertices, m, v, p, delta):
    opengl.glUseProgram(shader)
    opengl.glBindVertexArray(vao)
    camPosition = getCameraPosition(translate(position))

    opengl.glUniformMatrix4fv(opengl.glGetUniformLocation(shader, "model"), 1, opengl.GL_FALSE, m)
    opengl.glUniformMatrix4fv(opengl.glGetUniformLocation(shader, "view"), 1, opengl.GL_FALSE, v)
    opengl.glUniformMatrix4fv(opengl.glGetUniformLocation(shader, "projection"), 1, opengl.GL_FALSE, p)

    opengl.glUniform3f(opengl.glGetUniformLocation(shader, "cameraPosition"), camPosition[0], camPosition[1], camPosition[2])
    opengl.glUniform3f(opengl.glGetUniformLocation(shader, "lightPosition"), 0.0, 0.0, -10.0)

    opengl.glActiveTexture(opengl.GL_TEXTURE0)
    opengl.glBindTexture(opengl.GL_TEXTURE_2D, texture)
    opengl.glUniform1i(opengl.glGetUniformLocation(shader, "texture1"), 0)

    opengl.glDrawArrays(opengl.GL_TRIANGLES, 0, vertices)

    opengl.glBindTexture(opengl.GL_TEXTURE_2D, 0)
    opengl.glUseProgram(0)
    opengl.glBindVertexArray(0)"""

def bindTextureAttachment(texture_id, texture_data, width, height):
    opengl.glBindTexture(opengl.GL_TEXTURE_2D, texture_id)
    opengl.glTexImage2D(opengl.GL_TEXTURE_2D, 0, opengl.GL_RGBA, width, height, 0, opengl.GL_RGBA, opengl.GL_FLOAT, texture_data)
    opengl.glTexParameteri(opengl.GL_TEXTURE_2D, opengl.GL_TEXTURE_MIN_FILTER, opengl.GL_NEAREST)
    opengl.glTexParameteri(opengl.GL_TEXTURE_2D, opengl.GL_TEXTURE_MAG_FILTER, opengl.GL_NEAREST)
    opengl.glTexParameteri(opengl.GL_TEXTURE_2D, opengl.GL_TEXTURE_WRAP_S, opengl.GL_REPEAT)
    opengl.glTexParameteri(opengl.GL_TEXTURE_2D, opengl.GL_TEXTURE_WRAP_T, opengl.GL_REPEAT)
    opengl.glBindTexture(opengl.GL_TEXTURE_2D, 0)

def main():

    #program = createShader("shader.vert", "shader.frag")

    width, height = 640, 480
    aspect = width/float(height)

    opengl.glViewport(0, 0, width, height)
    opengl.glEnable(opengl.GL_DEPTH_TEST)
    opengl.glClearColor(0.5, 0.5, 0.5, 1.0)


    opengl.glClear(opengl.GL_COLOR_BUFFER_BIT | opengl.GL_DEPTH_BUFFER_BIT)
    #waves_lib.destroyWaves()
    print("Program terminated")

if __name__ == "__main__":
    print("Starting program")
    MainApp().run()
