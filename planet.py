"""Planet"""

import json
import numpy as np
from PIL import Image
from kivy.graphics import opengl
import math

class Planet(object):
    def __init__(self, config_path):
        with open(config_path, "r") as cfg_file:
            self.planet_params = json.loads(cfg_file.read())

        self.planet = {}
        self.init_planet(self.planet)
        if self.planet_params['hasAtmosphere']:
            self.planet['atmosphere'] = {}
            self.init_atmosphere(self.planet)
        self.planet['moons'] = []
        self.init_moons(self.planet)

        self.normal_map = None
        if len(self.planet_params['textures']['normalMap']) > 0:
            self.normal_map = self._load_texture(self.planet_params['textures']['normalMap'])
        self.specular_map = None
        if len(self.planet_params['textures']['specularMap']) > 0:
            self.specular_map = self._load_texture(self.planet_params['textures']['specularMap'])

    def init_planet(self, planet_dict):
        planet_dict['name'] = self.planet_params['name']
        planet_dict['semimajor_axis'] = self.planet_params['semiMajorAxis']/1000000.0
        planet_dict['scale'] = self.planet_params['radius']/100.0
        planet_dict['axial_tilt'] = self.planet_params['axialTilt']
        planet_dict['model'] = self.init_planet_model(planet_dict)
        planet_dict['texture_map'] = self._load_texture(self.planet_params['textures']['textureMap'])
        planet_dict['draw_atmosphere'] = self.planet_params['hasAtmosphere']

    def init_atmosphere(self, planet_dict):
        planet_dict['atmosphere']['radius'] = (self.planet_params['atmosphere']['radius']/self.planet_params['radius']+1.0)*planet_dict['scale']
        planet_dict['atmosphere']['model'] = self.init_atmosphere_model(planet_dict)
        planet_dict['atmosphere']['C_R'] = self.planet_params['atmosphere']['C_R']
        planet_dict['atmosphere']['K_R'] = self.planet_params['atmosphere']['K_R']
        planet_dict['atmosphere']['K_M'] = self.planet_params['atmosphere']['K_M']
        planet_dict['atmosphere']['G_M'] = self.planet_params['atmosphere']['G_M']
        planet_dict['atmosphere']['E'] = self.planet_params['atmosphere']['E']

    def init_planet_model(self, planet_dict):
        model = np.identity(4, dtype=np.float32)
        translation_matrix = np.identity(4, dtype=np.float32)
        rotation_matrix = np.identity(4, dtype=np.float32)

        rotation_matrix[2][2] = rotation_matrix[0][0] = math.cos(planet_dict['axial_tilt'])
        rotation_matrix[0][2] = math.sin(planet_dict['axial_tilt'])
        rotation_matrix[2][0] = -rotation_matrix[0][2]

        model[0][0] = planet_dict['scale']
        model[1][1] = planet_dict['scale']
        model[2][2] = planet_dict['scale']

        translation_matrix[0][3] = planet_dict['semimajor_axis']
        translation_matrix[2][3] = planet_dict['semimajor_axis']

        return np.dot(np.dot(translation_matrix, rotation_matrix), model)

    def init_atmosphere_model(self, planet_dict):
        model = np.identity(4, dtype=np.float32)
        translation_matrix = np.identity(4, dtype=np.float32)

        model[0][0] = planet_dict['atmosphere']['radius']
        model[1][1] = planet_dict['atmosphere']['radius']
        model[2][2] = planet_dict['atmosphere']['radius']

        translation_matrix[0][3] = planet_dict['semimajor_axis']
        translation_matrix[2][3] = planet_dict['semimajor_axis']

        return np.dot(translation_matrix, model)

    def init_moons(self, planet_dict):
        for moon in self.planet_params['moons']:
            temp_moon_dict = {}
            temp_moon_dict['name'] = moon['name']
            temp_moon_dict['semimajor_axis'] = moon['semiMajorAxis']/3000.0
            temp_moon_dict['scale'] = moon['radius']/100.0
            temp_moon_dict['axial_tilt'] = moon['axialTilt']
            temp_moon_dict['texture_map'] = self._load_texture(moon['textures']['textureMap'])

            model = np.identity(4, dtype=np.float32)
            translation_matrix = np.identity(4, dtype=np.float32)
            rotation_matrix = np.identity(4, dtype=np.float32)

            rotation_matrix[2][2] = rotation_matrix[0][0] = math.cos(temp_moon_dict['axial_tilt'])
            rotation_matrix[0][2] = math.sin(temp_moon_dict['axial_tilt'])
            rotation_matrix[2][0] = -rotation_matrix[0][2]
            model[0][0] = temp_moon_dict['scale']
            model[1][1] = temp_moon_dict['scale']
            model[2][2] = temp_moon_dict['scale']

            translation_matrix[0][3] = temp_moon_dict['semimajor_axis']+self.planet['model'][0][3]
            translation_matrix[1][3] = self.planet['model'][1][3]
            translation_matrix[2][3] = temp_moon_dict['semimajor_axis']+self.planet['model'][2][3]

            temp_moon_dict['model'] = np.dot(np.dot(translation_matrix, rotation_matrix), model)
            planet_dict['moons'].append(temp_moon_dict)
            
    def update_planet_model(self, time):
        rotation_matrix = np.identity(4, dtype=np.float32)
        rotation_matrix[2][2] = rotation_matrix[0][0] = math.cos(time)
        rotation_matrix[0][2] = math.sin(time)
        rotation_matrix[2][0] = -rotation_matrix[0][2]

        self.planet['model'] = np.dot(self.init_planet_model(self.planet), rotation_matrix)

        #self.planet['model'][0][3] = self.planet_params['semiMajorAxis']/1000000.0*math.cos(time)
        #self.planet['model'][2][3] = self.planet_params['semiMajorAxis']/1000000.0*math.sin(time)

    def update_moon_models(self, time):
        for moon in self.planet['moons']:
            moon['model'][0][3] = (self.planet['model'][0][3])+moon['semimajor_axis']*math.cos(time)
            moon['model'][2][3] = (self.planet['model'][2][3])+moon['semimajor_axis']*math.sin(time)

    def draw(self, arcball_camera, shader_program, buffer_object, perspective_arr, view_arr, atmosphere_program, atmosphere_buffer, time, depthmap=None):
        opengl.glUseProgram(shader_program)
        #planet.update_planet_model(self.global_time/10.0)

        opengl.glBindBuffer(opengl.GL_ARRAY_BUFFER, buffer_object.vbo)

        self.update_planet_model(time)
        self.draw_sphere_object(arcball_camera, self.planet, shader_program, buffer_object, perspective_arr, view_arr, depthmap)
        self.draw_moons(arcball_camera, shader_program, buffer_object, perspective_arr, view_arr, time, depthmap)

        if self.planet['draw_atmosphere']:
            self.draw_atmosphere(arcball_camera, atmosphere_program, atmosphere_buffer, perspective_arr, view_arr)
    
    def draw_moons(self, arcball_camera, shader_program, buffer_object, perspective_arr, view_arr, time, depthmap):
        opengl.glBindBuffer(opengl.GL_ARRAY_BUFFER, buffer_object.vbo)
        self.update_moon_models(time)

        for moon in self.planet['moons']:
            self.draw_sphere_object(arcball_camera, moon, shader_program, buffer_object, perspective_arr, view_arr, depthmap)

    def draw_sphere_object(self, arcball_camera, object_dict, shader_program, buffer_object, perspective_arr, view_arr, depthmap):
        u_loc = opengl.glGetUniformLocation(shader_program, b"projection")
        opengl.glUniformMatrix4fv(u_loc, 1, False, np.array(perspective_arr).flatten().tobytes())
        view_loc = opengl.glGetUniformLocation(shader_program, b"view")
        opengl.glUniformMatrix4fv(view_loc, 1, False, view_arr.flatten().tobytes())
        model_loc = opengl.glGetUniformLocation(shader_program, b"model")
        opengl.glUniformMatrix4fv(model_loc, 1, False, object_dict['model'].flatten().tobytes())

        camera_position = arcball_camera.camera_model_view_position(list(object_dict['model'].flatten()))
        opengl.glUniform3f(opengl.glGetUniformLocation(shader_program, b"camera_position"), camera_position[0], camera_position[1], camera_position[2])
        opengl.glUniform3f(opengl.glGetUniformLocation(shader_program, b"lightPosition"), 10.0, 5.0, -4.0)

        opengl.glActiveTexture(opengl.GL_TEXTURE1)
        opengl.glBindTexture(opengl.GL_TEXTURE_2D, object_dict['texture_map'])
        opengl.glUniform1i(opengl.glGetUniformLocation(shader_program, b"texture1"), 1)
        if self.normal_map != None:
            opengl.glActiveTexture(opengl.GL_TEXTURE2)
            opengl.glBindTexture(opengl.GL_TEXTURE_2D, self.normal_map)
            opengl.glUniform1i(opengl.glGetUniformLocation(shader_program, b"normal_map"), 2)
        if self.specular_map != None:
            opengl.glActiveTexture(opengl.GL_TEXTURE3)
            opengl.glBindTexture(opengl.GL_TEXTURE_2D, self.specular_map)
            opengl.glUniform1i(opengl.glGetUniformLocation(shader_program, b"specular_map"), 3)
        if depthmap != None:
            opengl.glActiveTexture(opengl.GL_TEXTURE4)
            opengl.glBindTexture(opengl.GL_TEXTURE_2D, depthmap)
            opengl.glUniform1i(opengl.glGetUniformLocation(shader_program, b"depthmap"), 4)
        opengl.glActiveTexture(opengl.GL_TEXTURE0)
        
        opengl.glEnableVertexAttribArray(0)
        opengl.glVertexAttribPointer(0, 3, opengl.GL_FLOAT, False, 12, 0)
        opengl.glEnableVertexAttribArray(1)
        opengl.glVertexAttribPointer(1, 3, opengl.GL_FLOAT, False, 12, buffer_object.point_nbytes)
        opengl.glEnableVertexAttribArray(2)
        opengl.glVertexAttribPointer(2, 3, opengl.GL_FLOAT, False, 12, buffer_object.point_nbytes+buffer_object.tangent_nbytes)
        opengl.glDrawArrays(opengl.GL_TRIANGLES, 0, buffer_object.num_vertices)

    def draw_atmosphere(self, arcball_camera, shader_program, buffer, perspective_arr, view_arr):
        opengl.glEnable(opengl.GL_BLEND)
        opengl.glBlendFunc(opengl.GL_ONE, opengl.GL_ONE)
        opengl.glUseProgram(shader_program)

        #model_arr = np.array(self.arcball_camera.view_translation, dtype=np.float32).reshape(4, 4)
        #model_arr = np.array(model, dtype=np.float32).reshape(4, 4)
        opengl.glBindBuffer(opengl.GL_ARRAY_BUFFER, buffer.vbo)

        opengl.glUniformMatrix4fv(opengl.glGetUniformLocation(shader_program, b"projection"), 1, False, np.array(perspective_arr).flatten().tobytes())
        opengl.glUniformMatrix4fv(opengl.glGetUniformLocation(shader_program, b"view"), 1, False, view_arr.flatten().tobytes())
        opengl.glUniformMatrix4fv(opengl.glGetUniformLocation(shader_program, b"model"), 1, False, self.planet['atmosphere']['model'].flatten().tobytes())

        # Atmosphere constants
        opengl.glUniform1f(opengl.glGetUniformLocation(shader_program, b"fInnerRadius"), self.planet['scale'])
        opengl.glUniform1f(opengl.glGetUniformLocation(shader_program, b"fOuterRadius"), self.planet['atmosphere']['radius'])
        opengl.glUniform3f(opengl.glGetUniformLocation(shader_program, b"C_R"), self.planet['atmosphere']['C_R'][0], self.planet['atmosphere']['C_R'][1], self.planet['atmosphere']['C_R'][2])
        opengl.glUniform1f(opengl.glGetUniformLocation(shader_program, b"K_R"), self.planet['atmosphere']['K_R'])
        opengl.glUniform1f(opengl.glGetUniformLocation(shader_program, b"K_M"), self.planet['atmosphere']['K_M'])
        opengl.glUniform1f(opengl.glGetUniformLocation(shader_program, b"G_M"), self.planet['atmosphere']['G_M'])
        opengl.glUniform1f(opengl.glGetUniformLocation(shader_program, b"E"), self.planet['atmosphere']['E'])
        
        camera_position = arcball_camera.camera_model_view_position(list(self.planet['atmosphere']['model'].flatten()))
        opengl.glUniform3f(opengl.glGetUniformLocation(shader_program, b"camPosition"), camera_position[0], camera_position[1], camera_position[2])
        opengl.glUniform3f(opengl.glGetUniformLocation(shader_program, b"lightPosition"), 10.0, 5.0, -4.0)

        opengl.glEnableVertexAttribArray(0)
        opengl.glVertexAttribPointer(0, 3, opengl.GL_FLOAT, False, 12, 0)
        opengl.glEnableVertexAttribArray(1)
        opengl.glVertexAttribPointer(1, 3, opengl.GL_FLOAT, False, 12, buffer.point_nbytes)
        opengl.glDrawArrays(opengl.GL_TRIANGLES, 0, buffer.num_vertices)

        opengl.glBlendFunc(opengl.GL_SRC_ALPHA, opengl.GL_ONE_MINUS_SRC_ALPHA)
        #opengl.glDisable(opengl.GL_BLEND)

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