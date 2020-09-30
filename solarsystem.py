import os
import math
import numpy as np

from kivy.graphics import opengl
from PIL import Image
from planet import Planet
from shader import Shader

class SolarSystem(object):
    def __init__(self):
        self.init_shaders()

        self.sun = {}
        model = np.identity(4, dtype=np.float32)
        self.sun['name'] = 'sun'
        self.sun['semimajor_axis'] = 1.0
        self.sun['scale'] = 20.0
        self.sun['axial_tilt'] = 1.0
        
        self.sun['texture_map'] = self._load_texture("resources/textures/8k_sun.jpg")
        self.skybox_texture = self._load_texture("resources/skybox/milkyway.jpg")

        model[0][0] = self.sun['scale']
        model[1][1] = self.sun['scale']
        model[2][2] = self.sun['scale']
        self.sun['model'] = model

        self.earth = Planet("config/earth.json")
        self.mars = Planet("config/mars.json")


    def init_shaders(self):
        self.star_shader = Shader("shaders/star.vert", "shaders/star.frag").get_program()
        self.planet_shader = Shader("shaders/planet.vert", "shaders/planet.frag").get_program()
        self.atmosphere_shader = Shader("shaders/atmosphere.vert", "shaders/atmosphere.frag").get_program()
        self.skybox_shader = Shader("shaders/skybox.vert", "shaders/skybox.frag").get_program()


    def draw(self, arcball_camera, buffer_object, perspective_arr, view_arr, atmosphere_buffer, time, depthmap=None):
        opengl.glUseProgram(self.star_shader)
        #planet.update_planet_model(self.global_time/10.0)

        opengl.glBindBuffer(opengl.GL_ARRAY_BUFFER, buffer_object.vbo)

        self.draw_sphere_object(arcball_camera, self.sun, self.star_shader, buffer_object, perspective_arr, view_arr, time)

        self.earth.draw(arcball_camera, self.planet_shader, buffer_object, perspective_arr, view_arr, self.atmosphere_shader, atmosphere_buffer, time)
        self.mars.draw(arcball_camera, self.planet_shader, buffer_object, perspective_arr, view_arr, self.atmosphere_shader, atmosphere_buffer, time)

        #self.draw_skybox(arcball_camera, perspective_arr, view_arr, self.skybox_shader, atmosphere_buffer)
        

    def draw_sphere_object(self, arcball_camera, object_dict, shader_program, buffer_object, perspective_arr, view_arr, time):
        opengl.glEnable(opengl.GL_BLEND)
        opengl.glBlendFunc(opengl.GL_ONE, opengl.GL_ONE)
        opengl.glUseProgram(shader_program)

        u_loc = opengl.glGetUniformLocation(shader_program, b"projection")
        opengl.glUniformMatrix4fv(u_loc, 1, False, np.array(perspective_arr).flatten().tobytes())
        view_loc = opengl.glGetUniformLocation(shader_program, b"view")
        opengl.glUniformMatrix4fv(view_loc, 1, False, view_arr.flatten().tobytes())
        model_loc = opengl.glGetUniformLocation(shader_program, b"model")
        opengl.glUniformMatrix4fv(model_loc, 1, False, object_dict['model'].flatten().tobytes())

        camera_position = arcball_camera.camera_model_view_position(list(object_dict['model'].flatten()))
        opengl.glUniform3f(opengl.glGetUniformLocation(shader_program, b"camera_position"), camera_position[0], camera_position[1], camera_position[2])
        opengl.glUniform3f(opengl.glGetUniformLocation(shader_program, b"lightPosition"), 10.0, 5.0, -4.0)
        opengl.glUniform1f(opengl.glGetUniformLocation(shader_program, b"time"), time)
        opengl.glUniform1f(opengl.glGetUniformLocation(shader_program, b"fInnerRadius"), self.sun['scale'])

        opengl.glActiveTexture(opengl.GL_TEXTURE1)
        opengl.glBindTexture(opengl.GL_TEXTURE_2D, object_dict['texture_map'])
        opengl.glUniform1i(opengl.glGetUniformLocation(shader_program, b"texture1"), 1)
        opengl.glActiveTexture(opengl.GL_TEXTURE0)

        opengl.glEnableVertexAttribArray(0)
        opengl.glVertexAttribPointer(0, 3, opengl.GL_FLOAT, False, 12, 0)
        opengl.glEnableVertexAttribArray(1)
        opengl.glVertexAttribPointer(1, 3, opengl.GL_FLOAT, False, 12, buffer_object.point_nbytes)
        opengl.glEnableVertexAttribArray(2)
        opengl.glVertexAttribPointer(2, 3, opengl.GL_FLOAT, False, 12, buffer_object.point_nbytes+buffer_object.tangent_nbytes)
        opengl.glDrawArrays(opengl.GL_TRIANGLES, 0, buffer_object.num_vertices)

        opengl.glBlendFunc(opengl.GL_SRC_ALPHA, opengl.GL_ONE_MINUS_SRC_ALPHA)


    def draw_skybox(self, arcball_camera, perspective_arr, view_arr, shader, buffer):
        opengl.glUseProgram(shader)
        opengl.glDepthMask(opengl.GL_FALSE)
        opengl.glDepthFunc(opengl.GL_LEQUAL)

        opengl.glBindBuffer(opengl.GL_ARRAY_BUFFER, buffer.vbo)

        opengl.glUniformMatrix4fv(opengl.glGetUniformLocation(shader, b"projection"), 1, False, np.array(perspective_arr).flatten().tobytes())
        opengl.glUniformMatrix4fv(opengl.glGetUniformLocation(shader, b"view"), 1, False, view_arr.flatten().tobytes())

        opengl.glActiveTexture(opengl.GL_TEXTURE1)
        opengl.glBindTexture(opengl.GL_TEXTURE_2D, self.skybox_texture)
        opengl.glUniform1i(opengl.glGetUniformLocation(shader, b"skybox"), 1)
        opengl.glActiveTexture(opengl.GL_TEXTURE0)

        opengl.glEnableVertexAttribArray(0)
        opengl.glVertexAttribPointer(0, 3, opengl.GL_FLOAT, False, 12, 0)
        opengl.glEnableVertexAttribArray(1)
        opengl.glVertexAttribPointer(1, 3, opengl.GL_FLOAT, False, 12, buffer.point_nbytes)
        opengl.glDrawArrays(opengl.GL_TRIANGLES, 0, buffer.num_vertices)

        opengl.glDepthMask(opengl.GL_TRUE)


    def _load_texture(self, path):
        img = Image.open(path)
        #img = Image.open("resources/textures/earth_day.jpg")
        img_data = np.array(list(img.getdata()), np.uint8)
        (texture,) = opengl.glGenTextures(1)
        opengl.glBindTexture(opengl.GL_TEXTURE_2D, texture)
        opengl.glTexParameteri(opengl.GL_TEXTURE_2D, opengl.GL_TEXTURE_WRAP_S, opengl.GL_REPEAT)
        opengl.glTexParameteri(opengl.GL_TEXTURE_2D, opengl.GL_TEXTURE_WRAP_T, opengl.GL_REPEAT)
        opengl.glTexParameteri(opengl.GL_TEXTURE_2D, opengl.GL_TEXTURE_MAG_FILTER, opengl.GL_LINEAR)
        opengl.glTexParameteri(opengl.GL_TEXTURE_2D, opengl.GL_TEXTURE_MIN_FILTER, opengl.GL_LINEAR)
        opengl.glTexImage2D(opengl.GL_TEXTURE_2D, 0, opengl.GL_RGB, img.width, img.height, 0, opengl.GL_RGB, opengl.GL_UNSIGNED_BYTE, img_data.tobytes())
        opengl.glEnable(opengl.GL_TEXTURE_2D)
        opengl.glBindTexture(opengl.GL_TEXTURE_2D, 0)

        return texture