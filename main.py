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
from kivy.uix.treeview import TreeView, TreeViewLabel

from shader import Shader
from buffer import Buffer
from planet import Planet
from solarsystem import SolarSystem

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
        self.setup_depthbuffer()

        self._keyboard = Window.request_keyboard(self._keyboard_closed, self)
        self._keyboard.bind(on_key_down=self._on_keyboard_down)

        self.arcball_camera = camera.Camera()
        self.perspective_matrix = self.arcball_camera.camera_perspective_matrix(90.0, 640.0/480.0, 0.1, 500.0)
        self.global_time = 0

        opengl.glEnable(opengl.GL_CULL_FACE)
        opengl.glCullFace(opengl.GL_BACK)
        #opengl.glEnable(opengl.GL_FRAMEBUFFER_SRGB)
        
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

    def update_glsl(self, *largs):
        self.angle += 0.01
        self.draw_depthmap()
        self.blit_depth_fbo()


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

    def create_depth_texture(self, sizex, sizey):
        (texture,) = opengl.glGenTextures(1)
        opengl.glBindTexture(opengl.GL_TEXTURE_2D, texture)
        opengl.glTexParameteri(opengl.GL_TEXTURE_2D, opengl.GL_TEXTURE_WRAP_S, opengl.GL_REPEAT)
        opengl.glTexParameteri(opengl.GL_TEXTURE_2D, opengl.GL_TEXTURE_WRAP_T, opengl.GL_REPEAT)
        opengl.glTexParameteri(opengl.GL_TEXTURE_2D, opengl.GL_TEXTURE_MAG_FILTER, opengl.GL_NEAREST)
        opengl.glTexParameteri(opengl.GL_TEXTURE_2D, opengl.GL_TEXTURE_MIN_FILTER, opengl.GL_NEAREST)
        opengl.glTexImage2D(opengl.GL_TEXTURE_2D, 0, opengl.GL_DEPTH_COMPONENT, sizex, sizey, 0, opengl.GL_DEPTH_COMPONENT, opengl.GL_FLOAT, (np.ones(sizex*sizey*4, np.uint8)*128).tobytes())
        #opengl.glEnable(opengl.GL_TEXTURE_2D)
        opengl.glBindTexture(opengl.GL_TEXTURE_2D, 0)

        return texture
        
    def setup_depthbuffer(self):
        (self.depthbuffer_id,) = opengl.glGenFramebuffers(1)
        opengl.glBindFramebuffer(opengl.GL_FRAMEBUFFER, self.depthbuffer_id)
        self.depthmap = self.create_depth_texture(1024, 1024)
        opengl.glFramebufferTexture2D(opengl.GL_FRAMEBUFFER, opengl.GL_DEPTH_ATTACHMENT, opengl.GL_TEXTURE_2D, self.depthmap, 0)
        #opengl.glDrawBuffer(opengl.GL_NONE)
        #opengl.glReadBuffer(opengl.GL_NONE)
        opengl.glBindFramebuffer(opengl.GL_FRAMEBUFFER, 0)


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
    
        #self.quad_vertices = np.array([-1.0, 1.0, -1.0, -1.0, 1.0, -1.0, -1.0, 1.0, 1.0, -1.0,1.0, 1.0], dtype=np.float32)
        #self.quad_tex =  np.array([0.0, 1.0, 0.0, 0.0,1.0, 0.0, 0.0, 1.0,1.0, 0.0,1.0, 1.0], dtype=np.float32)
        #self.screen_vbo = Buffer().add_buffer_data(self.quad_vertices, self.quad_tex)

    def init_shaders(self):
        self.program = Shader("shaders/planet.vert", "shaders/planet.frag").get_program()
        self.atmosphere_program = Shader("shaders/atmosphere.vert", "shaders/atmosphere.frag").get_program()
        self.screen_program = Shader("shaders/screen.vert", "shaders/screen.frag").get_program()
        self.star_shader = Shader("shaders/star.vert", "shaders/star.frag").get_program()

        self.shadow_shader = Shader("shaders/shadow.vert", "shaders/shadow.frag").get_program()
        self.skybox_shader = Shader("shaders/skybox.vert", "shaders/skybox.frag").get_program()

    def reload_shaders(self):
        #self.shadow_shader = Shader("shaders/shadow.vert", "shaders/shadow.frag").get_program()
        self.star_shader = Shader("shaders/star.vert", "shaders/star.frag").get_program()
        #self.program = Shader("shaders/planet.vert", "shaders/planet.frag").get_program()

    def init_objects(self):
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

        self.skybox_texture = self.load_texture("resources/skybox/milkyway.jpg")
        #self.mercury = Planet("config/mercury.json")
        #self.venus = Planet("config/venus.json")
        self.earth = Planet("config/earth.json")
        self.mars = Planet("config/mars.json")
        self.jupiter = Planet("config/jupiter.json")

        self.sun = SolarSystem()
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

        return texture

    def blit_fbo(self):
        opengl.glBindFramebuffer(opengl.GL_FRAMEBUFFER, self.glfboid)
        pixels = opengl.glReadPixels(0, 0, self.w, self.h, opengl.GL_RGBA, opengl.GL_UNSIGNED_BYTE)
        opengl.glBindFramebuffer(opengl.GL_FRAMEBUFFER, self.kvfboid)
        opengl.glTexSubImage2D(opengl.GL_TEXTURE_2D, 0, 0, 0, self.w, self.h, opengl.GL_RGBA, opengl.GL_UNSIGNED_BYTE, pixels)
        opengl.glBindFramebuffer(opengl.GL_FRAMEBUFFER, 0)
    
    def blit_depth_fbo(self):
        opengl.glBindFramebuffer(opengl.GL_FRAMEBUFFER, self.depthbuffer_id)
        pixels = opengl.glReadPixels(0, 0, 1024, 1024, opengl.GL_RGBA, opengl.GL_UNSIGNED_BYTE)
        opengl.glBindFramebuffer(opengl.GL_FRAMEBUFFER, self.kvfboid)
        opengl.glTexSubImage2D(opengl.GL_TEXTURE_2D, 0, 0, 0, self.w, self.h, opengl.GL_RGBA, opengl.GL_UNSIGNED_BYTE, pixels)
        opengl.glBindFramebuffer(opengl.GL_FRAMEBUFFER, 0)

    def draw_depthmap(self):
        opengl.glUseProgram(self.shadow_shader)
        time_theta = self.global_time/200.0
        perspective_arr = np.array(self.perspective_matrix, dtype=np.float32).reshape(4, 4)
        view_arr = np.array(self.arcball_camera.view_matrix, dtype=np.float32).reshape(4, 4)
        opengl.glViewport(0, 0, 1024, 1024)
        opengl.glBindFramebuffer(opengl.GL_FRAMEBUFFER, self.depthbuffer_id)
        opengl.glClear(opengl.GL_DEPTH_BUFFER_BIT)
        self.earth.draw(self.arcball_camera, self.program, self.sphere_buffer, perspective_arr, view_arr, self.atmosphere_program, self.atmosphere_buffer, time_theta, self.depthmap)
        self.mars.draw(self.arcball_camera, self.program, self.sphere_buffer, perspective_arr, view_arr, self.atmosphere_program, self.atmosphere_buffer, time_theta, self.depthmap)
        opengl.glBindFramebuffer(opengl.GL_FRAMEBUFFER, 0)

    def draw_fbo(self, targetfbo):
        time_theta = self.global_time/200.0
        perspective_arr = np.array(self.perspective_matrix, dtype=np.float32).reshape(4, 4)
        view_arr = np.array(self.arcball_camera.view_matrix, dtype=np.float32).reshape(4, 4)

        opengl.glViewport(0, 0, self.w, self.h)
        opengl.glEnable(opengl.GL_DEPTH_TEST)
        #opengl.glDepthMask(opengl.GL_FALSE)
        opengl.glDepthFunc(opengl.GL_LESS)

        opengl.glBindFramebuffer(opengl.GL_FRAMEBUFFER, targetfbo)
        opengl.glClearColor(0.1, 0.1, 0.1, 1.0)
        opengl.glClear(opengl.GL_COLOR_BUFFER_BIT | opengl.GL_DEPTH_BUFFER_BIT)

        #self.draw_skybox()

        self.earth.update_planet_model(time_theta)
        self.mars.update_planet_model(time_theta)
        #self.draw_planet(self.mercury, self.program, self.sphere_buffer, perspective_arr, view_arr)
        #self.draw_planet(self.venus, self.program, self.sphere_buffer, perspective_arr, view_arr)
        #self.draw_planet(self.arcball_camera, self.earth, self.program, self.sphere_buffer, perspective_arr, view_arr, self.atmosphere_program, self.atmosphere_buffer)
        self.sun.draw(self.arcball_camera, self.star_shader, self.sphere_buffer, perspective_arr, view_arr, self.atmosphere_program, self.atmosphere_buffer, time_theta)
        self.earth.draw(self.arcball_camera, self.program, self.sphere_buffer, perspective_arr, view_arr, self.atmosphere_program, self.atmosphere_buffer, time_theta)
        self.mars.draw(self.arcball_camera, self.program, self.sphere_buffer, perspective_arr, view_arr, self.atmosphere_program, self.atmosphere_buffer, time_theta)
        #self.jupiter.draw(self.arcball_camera, self.program, self.sphere_buffer, perspective_arr, view_arr, self.atmosphere_program, self.atmosphere_buffer, time_theta)
        #self.draw_planet(self.jupiter, self.program)
        #self.draw_planet(self.saturn, self.program)
        

        opengl.glBindFramebuffer(opengl.GL_FRAMEBUFFER, 0)

        opengl.glDisable(opengl.GL_DEPTH_TEST)

    def draw_skybox(self):
        opengl.glUseProgram(self.skybox_shader)
        opengl.glDepthMask(opengl.GL_FALSE)
        opengl.glDepthFunc(opengl.GL_LEQUAL)

        opengl.glBindBuffer(opengl.GL_ARRAY_BUFFER, self.atmosphere_buffer.vbo)

        perspective_arr = np.array(self.perspective_matrix, dtype=np.float32).reshape(4, 4)
        view_arr = np.array(self.arcball_camera.view_matrix, dtype=np.float32).reshape(4, 4)
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
        self.w.reload_shaders()

    def build(self):
        self.w = Application()
        #root = GridLayout(cols=1, padding=10)
        root = BoxLayout(orientation='vertical')
        #self.button = Button(text="Click for pop-up")
        #root.add_widget(self.button)

        tv = TreeView(root_options=dict(text='Solar System'),
                      hide_root=False,
                      indent_level=4)
        n1 = tv.add_node(TreeViewLabel(text=self.w.earth.planet['name'].capitalize()))
        n2 = tv.add_node(TreeViewLabel(text=self.w.mars.planet['name'].capitalize()))
        for sub_object in self.w.mars.planet['moons']:
            tv.add_node(TreeViewLabel(text=sub_object['name'].capitalize()), n2)
        for sub_object in self.w.earth.planet['moons']:
            tv.add_node(TreeViewLabel(text=sub_object['name'].capitalize()), n1)

        layout      = GridLayout(cols=1, padding=10)
        #popupLabel  = Label(text  = "Click for pop-up")
        closeButton = Button(text = "Reload\nShaders", size=(200, 100), on_press=self.onButtonPress)
        #layout.add_widget(Slider(value_track=True, value_track_color=[1, 0, 0, 1]))
        layout.add_widget(tv)
        layout.add_widget(closeButton)
        popup = Popup(title='Demo Popup',size_hint=(None, None), size=(200, 400), pos_hint={'top':.97,'right':.97},
                      content=layout, auto_dismiss=False)
        popup.open()  
        

        layout2 = BoxLayout(opacity=0.5)
        #pb = ProgressBar(max=1000, size=(100, 100))
        #pb.value = 750
        #layout2.add_widget(pb)
        #layout2.add_widget(Slider(value_track=True, value_track_color=[1, 0, 0, 1], size=(100, 100)))
        #layout2.add_widget(Button(text='Test Button', size=(100, 100)))
        
        # Draw the scene
        layout2.add_widget(self.w)
        root.add_widget(layout2)
    
        return root

if __name__ == "__main__":
    print("Starting program")
    MainApp().run()
