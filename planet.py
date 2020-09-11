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

        #if len(self.planet_params['textures']['normalMap']) > 0:
        #    self.normal_map = self._load_texture(self.planet_params['textures']['normalMap'])
        #if len(self.planet_params['textures']['specularMap']) > 0:
        #    self.specular_map = self._load_texture(self.planet_params['textures']['specularMap'])

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
        self.planet['model'][0][3] = self.planet_params['semiMajorAxis']/1000000.0*math.cos(time)
        self.planet['model'][2][3] = self.planet_params['semiMajorAxis']/1000000.0*math.sin(time)

    def update_moon_models(self, time):
        for moon in self.planet['moons']:
            moon['model'][0][3] = (self.planet['model'][0][3])+moon['semimajor_axis']*math.cos(time)
            moon['model'][2][3] = (self.planet['model'][2][3])+moon['semimajor_axis']*math.sin(time)

    def draw_planet(self):
        pass

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