"""Main"""
# pylint: disable=C0111
# pylint: disable=C0301
import os
import sys
import math
import ctypes
import time
import numpy as np
from PIL import Image

import kivy

#pylint: disable=wrong-import-position
kivy.require('1.1.1')
from kivy.app import App
from kivy.clock import Clock
from kivy.core.window import Window
from kivy.uix.widget import Widget
from kivy.uix.gridlayout import GridLayout
from kivy.uix.modalview import ModalView
from kivy.uix.popup import Popup
from kivy.graphics import Fbo
from kivy.graphics import Rectangle
from kivy.graphics import opengl
from kivy.uix.button import Button
from kivy.uix.boxlayout import BoxLayout
from kivy.uix.label import Label
from kivy.uix.textinput import TextInput
from kivy.uix.checkbox import CheckBox
from kivy.uix.slider import Slider
from kivy.uix.progressbar import ProgressBar

from shader import Shader
from buffer import Buffer
from planet import Planet

sys.path.insert(1, os.getcwd()+'/bin')

import waves
import geometry
import camera

#pylint: enable=wrong-import-position

"""Application"""
class Application(Widget):
    def __init__(self, **kwargs):
        super(Application, self).__init__(**kwargs)
        self.angle = 0
        self.setup_kvfbo()
        self.setup_glfbo()
        self.init_objects()
        self.init_shaders()

        self._keyboard = Window.request_keyboard(self._keyboard_closed, self)
        self._keyboard.bind(on_key_down=self._on_keyboard_down)

        self.arcball_camera = camera.Camera()
        self.perspective_matrix = self.arcball_camera.camera_perspective_matrix(90.0, 640.0/480.0, 0.1, 500.0)
        self.global_time = 0

        opengl.glEnable(opengl.GL_CULL_FACE)
        opengl.glCullFace(opengl.GL_BACK)
        
        #self.wave_patch = waves.Waves(dimension=128)
        #print(self.wave_patch.generate_waves(0.0))
        #dx_img_data = np.array(list(self.wave_patch.dx), np.uint8)
        #print(dx_img_data)
        #self.dx_wave_texture = self.create_texture(dx_img_data)

        Clock.schedule_interval(self.update_glsl, 1.0 / 60.0)


    def _keyboard_closed(self):
        self._keyboard.unbind(on_key_down=self._on_keyboard_down)
        self._keyboard = None

    def _on_keyboard_down(self, keyboard, keycode, text, modifiers):
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
        if touch.button == 'left':
            screen_position = list(touch.spos)
            self.arcball_camera.process_mouse_movement(screen_position[0], screen_position[1], 1)
        if touch.is_mouse_scrolling:
            if touch.button == 'scrolldown':
                self.arcball_camera.process_mouse_scroll(-0.01)
            elif touch.button == 'scrollup':
                self.arcball_camera.process_mouse_scroll(0.01)


    def on_touch_move(self, touch):
        screen_position = list(touch.spos)
        self.arcball_camera.process_mouse_movement(screen_position[0], screen_position[1], 0)

    #def on_touch_up(self, touch):
    #    print("Touch Release", touch)

    def update_glsl(self, *largs):
        self.angle += 0.01
        self.draw_fbo(self.glfboid)
        self.blit_fbo()
        self.canvas.ask_update()
        self.global_time += 1

    def setup_kvfbo(self):
        self.kvfbo = Fbo(with_depthbuffer=True, size=Window.size, compute_normal_mat=True)
        self.canvas.add(self.kvfbo)
        self.canvas.add(Rectangle(size=Window.size, texture=self.kvfbo.texture))
        self.kvfbo.bind()
        (self.kvfboid,) = opengl.glGetIntegerv(opengl.GL_FRAMEBUFFER_BINDING)
        self.kvfbo.release()

    def create_texture(self, img_data):
        (texture,) = opengl.glGenTextures(1)
        opengl.glBindTexture(opengl.GL_TEXTURE_2D, texture)
        opengl.glTexParameteri(opengl.GL_TEXTURE_2D, opengl.GL_TEXTURE_WRAP_S, opengl.GL_REPEAT)
        opengl.glTexParameteri(opengl.GL_TEXTURE_2D, opengl.GL_TEXTURE_WRAP_T, opengl.GL_REPEAT)
        opengl.glTexParameteri(opengl.GL_TEXTURE_2D, opengl.GL_TEXTURE_MAG_FILTER, opengl.GL_LINEAR)
        opengl.glTexParameteri(opengl.GL_TEXTURE_2D, opengl.GL_TEXTURE_MIN_FILTER, opengl.GL_LINEAR)
        opengl.glTexImage2D(opengl.GL_TEXTURE_2D, 0, opengl.GL_RGB, 128, 128, 0, opengl.GL_RGB, opengl.GL_UNSIGNED_BYTE, img_data.tobytes())
        opengl.glEnable(opengl.GL_TEXTURE_2D)
        opengl.glBindTexture(opengl.GL_TEXTURE_2D, 0)

        return texture

    def setup_glfbo(self):
        ## FBO initialisation
        self.w, self.h = Window.width, Window.height
        opengl.glEnable(opengl.GL_TEXTURE_2D)
        (self.glfboid,) = opengl.glGenFramebuffers(1)
        (self.gltexid,) = opengl.glGenTextures(1)
        opengl.glBindFramebuffer(opengl.GL_FRAMEBUFFER, self.glfboid)
        opengl.glBindTexture(opengl.GL_TEXTURE_2D, self.gltexid)

        opengl.glTexImage2D(opengl.GL_TEXTURE_2D, 0, opengl.GL_RGBA, self.w, self.h, 0, opengl.GL_RGBA, opengl.GL_UNSIGNED_BYTE, (np.ones(self.w*self.h*4, np.uint8)*128).tobytes())
        opengl.glFramebufferTexture2D(opengl.GL_FRAMEBUFFER, opengl.GL_COLOR_ATTACHMENT0, opengl.GL_TEXTURE_2D, self.gltexid, 0)
        
        (self.rbo,) = opengl.glGenRenderbuffers(1)
        opengl.glBindRenderbuffer(opengl.GL_RENDERBUFFER, self.rbo)
        opengl.glRenderbufferStorage(opengl.GL_RENDERBUFFER, opengl.GL_DEPTH_COMPONENT16, self.w, self.h)
        opengl.glFramebufferRenderbuffer(opengl.GL_FRAMEBUFFER, opengl.GL_DEPTH_ATTACHMENT, opengl.GL_RENDERBUFFER, self.rbo)
        if opengl.glCheckFramebufferStatus(opengl.GL_FRAMEBUFFER) != opengl.GL_FRAMEBUFFER_COMPLETE:
            print("ERROR::FRAMEBUFFER:: Framebuffer is not complete!")

        opengl.glBindFramebuffer(opengl.GL_FRAMEBUFFER, 0)
        opengl.glBindTexture(opengl.GL_TEXTURE_2D, 0)

        self.g = geometry.Geometry(subdivisions=1)
        
        self.g.quadcube(10)
        sphere = np.array(self.g.quadcube_points, dtype=np.float32)
        sphere_normals = np.array(self.g.quadcube_normals, dtype=np.float32)
        sphere_tangents = np.array(self.g.quadcube_tangents, dtype=np.float32)
        self.sphere_buffer = Buffer(sphere, sphere_normals, sphere_tangents)

        self.g.tetrahedron_sphere(4)
        atmosphere = np.array(self.g.tetrahedron_points, dtype=np.float32)
        atmosphere_normals = np.array(self.g.tetrahedron_normals, dtype=np.float32)
        self.atmosphere_buffer = Buffer(atmosphere, atmosphere_normals)
    
        #self.quad_vertices = np.array([-1.0, 1.0, -1.0, -1.0, 1.0, -1.0, -1.0, 1.0, 1.0, -1.0,1.0, 1.0], dtype=np.float32)
        #self.quad_tex =  np.array([0.0, 1.0, 0.0, 0.0,1.0, 0.0, 0.0, 1.0,1.0, 0.0,1.0, 1.0], dtype=np.float32)
        #self.screen_vbo = Buffer().add_buffer_data(self.quad_vertices, self.quad_tex)

    def init_shaders(self):
        self.program = Shader("shaders/planet.vert", "shaders/planet.frag").get_program()
        self.atmosphere_program = Shader("shaders/atmosphere.vert", "shaders/atmosphere.frag").get_program()
        self.screen_program = Shader("shaders/screen.vert", "shaders/screen.frag").get_program()

        self.skybox_shader = Shader("shaders/skybox.vert", "shaders/skybox.frag").get_program()

    def init_objects(self):
        self.skybox_texture = self.load_texture("resources/skybox/milkyway.jpg")
        #self.mercury = Planet("config/mercury.json")
        #self.venus = Planet("config/venus.json")
        self.earth = Planet("config/earth.json")
        #self.mars = Planet("config/mars.json")
        #self.jupiter = Planet("config/jupiter.json")
        #self.saturn = Planet("config/saturn.json")

    def load_texture(self, path):
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

        """GLuint loadCubemap(char **faces)
        {
        	GLuint textureID;
            glGenTextures(1, &textureID);
            glActiveTexture(GL_TEXTURE0);

            int width, height;
            unsigned char* image;

            glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
            for (GLuint i = 0; i < 6; i++)
            {
                image = SOIL_load_image(faces[i], &width, &height, 0, SOIL_LOAD_RGB);
                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);

            }
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
            glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

            return textureID;
        }"""

        return texture

    def blit_fbo(self):
        opengl.glBindFramebuffer(opengl.GL_FRAMEBUFFER, self.glfboid)
        pixels = opengl.glReadPixels(0, 0, self.w, self.h, opengl.GL_RGBA, opengl.GL_UNSIGNED_BYTE)
        opengl.glBindFramebuffer(opengl.GL_FRAMEBUFFER, self.kvfboid)
        opengl.glTexSubImage2D(opengl.GL_TEXTURE_2D, 0, 0, 0, self.w, self.h, opengl.GL_RGBA, opengl.GL_UNSIGNED_BYTE, pixels)
        opengl.glBindFramebuffer(opengl.GL_FRAMEBUFFER, 0)

    def draw_fbo(self, targetfbo):
        opengl.glEnable(opengl.GL_DEPTH_TEST)
        #opengl.glDepthMask(opengl.GL_FALSE)
        opengl.glDepthFunc(opengl.GL_LESS)

        opengl.glBindFramebuffer(opengl.GL_FRAMEBUFFER, targetfbo)
        opengl.glClearColor(0.0, 1.0, 1.0, 1.0)
        opengl.glClear(opengl.GL_COLOR_BUFFER_BIT | opengl.GL_DEPTH_BUFFER_BIT)

        #self.draw_skybox()

        perspective_arr = np.array(self.perspective_matrix, dtype=np.float32).reshape(4, 4)
        view_arr = np.array(self.arcball_camera.view_matrix, dtype=np.float32).reshape(4, 4)

        #self.draw_planet(self.mercury, self.program, self.sphere_buffer, perspective_arr, view_arr)
        #self.draw_planet(self.venus, self.program, self.sphere_buffer, perspective_arr, view_arr)
        self.draw_planet(self.earth, self.program, self.sphere_buffer, perspective_arr, view_arr)
        #self.draw_planet(self.mars, self.program, self.sphere_buffer, perspective_arr, view_arr)
        #self.draw_planet(self.jupiter, self.program)
        #self.draw_planet(self.saturn, self.program)
        
        opengl.glBindFramebuffer(opengl.GL_FRAMEBUFFER, 0)

        opengl.glDisable(opengl.GL_DEPTH_TEST)

    def draw_planet(self, planet, shader_program, buffer_object, perspective_arr, view_arr):
        opengl.glUseProgram(shader_program)
        #planet.update_planet_model(self.global_time/10.0)

        opengl.glBindBuffer(opengl.GL_ARRAY_BUFFER, buffer_object.vbo)

        self.draw_sphere_object(planet.planet, shader_program, buffer_object, perspective_arr, view_arr)
        self.draw_moons(planet, shader_program, buffer_object, perspective_arr, view_arr)

        #if planet.planet['draw_atmosphere']:
        #    self.draw_atmosphere(planet, self.atmosphere_program, perspective_arr, view_arr)
    
    def draw_moons(self, planet, shader_program, buffer_object, perspective_arr, view_arr):
        opengl.glBindBuffer(opengl.GL_ARRAY_BUFFER, buffer_object.vbo)
        planet.update_moon_models(self.global_time/100.0)

        for moon in planet.planet['moons']:
            self.draw_sphere_object(moon, shader_program, buffer_object, perspective_arr, view_arr)

    def draw_sphere_object(self, object_dict, shader_program, buffer_object, perspective_arr, view_arr):
        u_loc = opengl.glGetUniformLocation(shader_program, b"projection")
        opengl.glUniformMatrix4fv(u_loc, 1, False, np.array(perspective_arr).flatten().tobytes())
        view_loc = opengl.glGetUniformLocation(shader_program, b"view")
        opengl.glUniformMatrix4fv(view_loc, 1, False, view_arr.flatten().tobytes())
        model_loc = opengl.glGetUniformLocation(shader_program, b"model")
        opengl.glUniformMatrix4fv(model_loc, 1, False, object_dict['model'].flatten().tobytes())

        camera_position = self.arcball_camera.camera_model_view_position(list(object_dict['model'].flatten()))
        opengl.glUniform3f(opengl.glGetUniformLocation(shader_program, b"camera_position"), camera_position[0], camera_position[1], camera_position[2])
        opengl.glUniform3f(opengl.glGetUniformLocation(shader_program, b"lightPosition"),10.0, 5.0, -4.0)

        opengl.glActiveTexture(opengl.GL_TEXTURE1)
        opengl.glBindTexture(opengl.GL_TEXTURE_2D, object_dict['texture_map'])
        opengl.glUniform1i(opengl.glGetUniformLocation(shader_program, b"texture1"), 1)
        #opengl.glActiveTexture(opengl.GL_TEXTURE2)
        #opengl.glBindTexture(opengl.GL_TEXTURE_2D, planet.normal_map)
        #opengl.glUniform1i(opengl.glGetUniformLocation(shader_program, b"normal_map"), 2)
        #opengl.glActiveTexture(opengl.GL_TEXTURE3)
        #opengl.glBindTexture(opengl.GL_TEXTURE_2D, planet.specular_map)
        #opengl.glUniform1i(opengl.glGetUniformLocation(shader_program, b"specular_map"), 3)
        opengl.glActiveTexture(opengl.GL_TEXTURE0)
        
        opengl.glEnableVertexAttribArray(0)
        opengl.glVertexAttribPointer(0, 3, opengl.GL_FLOAT, False, 12, 0)
        opengl.glEnableVertexAttribArray(1)
        opengl.glVertexAttribPointer(1, 3, opengl.GL_FLOAT, False, 12, buffer_object.point_nbytes)
        opengl.glEnableVertexAttribArray(2)
        opengl.glVertexAttribPointer(2, 3, opengl.GL_FLOAT, False, 12, buffer_object.point_nbytes+buffer_object.tangent_nbytes)
        opengl.glDrawArrays(opengl.GL_TRIANGLES, 0, buffer_object.num_vertices)

    def draw_atmosphere(self, planet, shader_program, perspective_arr, view_arr):
        opengl.glEnable(opengl.GL_BLEND)
        opengl.glBlendFunc(opengl.GL_ONE, opengl.GL_ONE)
        opengl.glUseProgram(shader_program)

        #model_arr = np.array(self.arcball_camera.view_translation, dtype=np.float32).reshape(4, 4)
        #model_arr = np.array(model, dtype=np.float32).reshape(4, 4)
        opengl.glBindBuffer(opengl.GL_ARRAY_BUFFER, self.atmosphere_buffer.vbo)

        opengl.glUniformMatrix4fv(opengl.glGetUniformLocation(shader_program, b"projection"), 1, False, np.array(perspective_arr).flatten().tobytes())
        opengl.glUniformMatrix4fv(opengl.glGetUniformLocation(shader_program, b"view"), 1, False, view_arr.flatten().tobytes())
        opengl.glUniformMatrix4fv(opengl.glGetUniformLocation(shader_program, b"model"), 1, False, planet.planet['atmosphere']['model'].flatten().tobytes())

        # Atmosphere constants
        opengl.glUniform1f(opengl.glGetUniformLocation(shader_program, b"fInnerRadius"), planet.planet['scale'])
        opengl.glUniform1f(opengl.glGetUniformLocation(shader_program, b"fOuterRadius"), planet.planet['atmosphere']['radius'])
        opengl.glUniform3f(opengl.glGetUniformLocation(shader_program, b"C_R"), planet.planet['atmosphere']['C_R'][0], planet.planet['atmosphere']['C_R'][1], planet.planet['atmosphere']['C_R'][2])
        opengl.glUniform1f(opengl.glGetUniformLocation(shader_program, b"K_R"), planet.planet['atmosphere']['K_R'])
        opengl.glUniform1f(opengl.glGetUniformLocation(shader_program, b"K_M"), planet.planet['atmosphere']['K_M'])
        opengl.glUniform1f(opengl.glGetUniformLocation(shader_program, b"G_M"), planet.planet['atmosphere']['G_M'])
        opengl.glUniform1f(opengl.glGetUniformLocation(shader_program, b"E"), planet.planet['atmosphere']['E'])
        
        camera_position = self.arcball_camera.camera_model_view_position(list(planet.planet['atmosphere']['model'].flatten()))
        opengl.glUniform3f(opengl.glGetUniformLocation(shader_program, b"camPosition"), camera_position[0], camera_position[1], camera_position[2])
        opengl.glUniform3f(opengl.glGetUniformLocation(shader_program, b"lightPosition"), 10.0, 5.0, -4.0)

        opengl.glEnableVertexAttribArray(0)
        opengl.glVertexAttribPointer(0, 3, opengl.GL_FLOAT, False, 12, 0)
        opengl.glEnableVertexAttribArray(1)
        opengl.glVertexAttribPointer(1, 3, opengl.GL_FLOAT, False, 12, self.atmosphere_buffer.point_nbytes)
        opengl.glDrawArrays(opengl.GL_TRIANGLES, 0, self.atmosphere_buffer.num_vertices)

        opengl.glBlendFunc(opengl.GL_SRC_ALPHA, opengl.GL_ONE_MINUS_SRC_ALPHA)
        #opengl.glDisable(opengl.GL_BLEND)

    """def draw_screen(self, model, shader_program):
        opengl.glUseProgram(shader_program)
        perspective_arr = np.array(self.perspective_matrix, dtype=np.float32).reshape(4, 4)
        view_arr = np.array(self.arcball_camera.view_matrix, dtype=np.float32).reshape(4, 4)
        #model_arr = np.array(self.arcball_camera.view_translation, dtype=np.float32).reshape(4, 4)
        model_arr = np.array(model, dtype=np.float32).reshape(4, 4)
        opengl.glBindBuffer(opengl.GL_ARRAY_BUFFER, self.screen_vbo)

        u_loc = opengl.glGetUniformLocation(shader_program, b"projection")
        opengl.glUniformMatrix4fv(u_loc, 1, False, np.array(perspective_arr).flatten().tobytes())

        view_loc = opengl.glGetUniformLocation(shader_program, b"view")
        opengl.glUniformMatrix4fv(view_loc, 1, False, view_arr.flatten().tobytes())

        model_loc = opengl.glGetUniformLocation(shader_program, b"model")
        opengl.glUniformMatrix4fv(model_loc, 1, False, model_arr.flatten().tobytes())

        camera_position = self.arcball_camera.camera_model_view_position([1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 2.0, 0.0, 0.0, 0.0, 1.0])
        opengl.glUniform3f(opengl.glGetUniformLocation(shader_program, b"camera_position"), camera_position[0], camera_position[1], camera_position[2])

        opengl.glActiveTexture(opengl.GL_TEXTURE1)
        opengl.glBindTexture(opengl.GL_TEXTURE_2D, self.glfboid)
        opengl.glUniform1i(opengl.glGetUniformLocation(shader_program, b"screenTexture"), 0)
        
        opengl.glEnableVertexAttribArray(0)
        opengl.glVertexAttribPointer(0, 2, opengl.GL_FLOAT, False, 8, 0)
        opengl.glEnableVertexAttribArray(1)
        opengl.glVertexAttribPointer(1, 2, opengl.GL_FLOAT, False, 8, self.quad_vertices.nbytes)
        opengl.glDrawArrays(opengl.GL_TRIANGLES, 0, len(self.quad_vertices))"""

    def draw_skybox(self):
        opengl.glUseProgram(self.skybox_shader)
        opengl.glDepthMask(opengl.GL_FALSE)
        opengl.glDepthFunc(opengl.GL_LEQUAL)

        opengl.glBindBuffer(opengl.GL_ARRAY_BUFFER, self.atmosphere_buffer.vbo)

        perspective_arr = np.array(self.perspective_matrix, dtype=np.float32).reshape(4, 4)
        view_arr = np.array(self.arcball_camera.view_matrix, dtype=np.float32).reshape(4, 4)
        model_arr = np.identity(4)
        opengl.glUniformMatrix4fv(opengl.glGetUniformLocation(self.skybox_shader, b"projection"), 1, False, np.array(perspective_arr).flatten().tobytes())
        opengl.glUniformMatrix4fv(opengl.glGetUniformLocation(self.skybox_shader, b"view"), 1, False, view_arr.flatten().tobytes())

        opengl.glActiveTexture(opengl.GL_TEXTURE1)
        opengl.glBindTexture(opengl.GL_TEXTURE_2D, self.skybox_texture)
        opengl.glUniform1i(opengl.glGetUniformLocation(self.skybox_shader, b"skybox"), 1)
        opengl.glActiveTexture(opengl.GL_TEXTURE0)

        opengl.glEnableVertexAttribArray(0)
        opengl.glVertexAttribPointer(0, 3, opengl.GL_FLOAT, False, 12, 0)
        opengl.glEnableVertexAttribArray(1)
        opengl.glVertexAttribPointer(1, 3, opengl.GL_FLOAT, False, 12, self.atmosphere_buffer.point_nbytes)
        opengl.glDrawArrays(opengl.GL_TRIANGLES, 0, self.atmosphere_buffer.num_vertices)

        opengl.glDepthMask(opengl.GL_TRUE)


class MainApp(App):
    cwd = os.getcwd()
    os.chdir(cwd)

    def onButtonPress(self, button):
        print("asdf")

    def build(self):
        #root = GridLayout(cols=1, padding=10)
        root = BoxLayout(orientation='vertical')
        #self.button = Button(text="Click for pop-up")
        #root.add_widget(self.button)

        layout      = GridLayout(cols=1, padding=10)
        popupLabel  = Label(text  = "Click for pop-up")
        closeButton = Button(text = "Close the pop-up")
        layout.add_widget(Slider(value_track=True, value_track_color=[1, 0, 0, 1]))
        layout.add_widget(popupLabel)
        layout.add_widget(closeButton)
        popup = Popup(title='Demo Popup',size_hint=(None, None), size=(200, 400), pos_hint={'top':.97,'right':.97},
                      content=layout, auto_dismiss=False)
        popup.open()  
        w = Application()

        layout2 = BoxLayout(opacity=0.5)
        #pb = ProgressBar(max=1000, size=(100, 100))
        #pb.value = 750
        #layout2.add_widget(pb)
        #layout2.add_widget(Slider(value_track=True, value_track_color=[1, 0, 0, 1], size=(100, 100)))
        #layout2.add_widget(Button(text='Test Button', size=(100, 100)))
        
        # Draw the scene
        layout2.add_widget(w)
        root.add_widget(layout2)
    
        return root

if __name__ == "__main__":
    print("Starting program")
    MainApp().run()
