"""Common shader functions"""
from kivy.graphics import opengl

"""Shader"""
class Shader(object):
    def __init__(self):
        self.program = opengl.glCreateProgram()
        #shader_bytes = self.read_shader(path)
        #vertex = opengl.glCreateShader(opengl.GL_VERTEX_SHADER)

    def create_shader(self, path, shader_type):
        shader_code = self.read_shader(path)
        shader = opengl.glCreateShader(shader_type)
        opengl.glShaderSource(shader, shader_code)
        opengl.glCompileShader(shader)
        #self.link_shader(shader)
        return shader

    def link_shader(self, shader):
        opengl.glAttachShader(self.program, shader)
        opengl.glLinkProgram(self.program)
        opengl.glDetachShader(self.program, shader)

    def read_shader(self, path):
        with open(path, 'r') as f:
             return f.read().encode()

    def check_err(self, shader):
        vert_err = opengl.glGetShaderiv(shader, opengl.GL_COMPILE_STATUS)
        if vert_err != 0:
            print(opengl.glGetShaderInfoLog(shader, 1024).decode())

    def get_program(self):
        return self.program