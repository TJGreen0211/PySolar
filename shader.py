"""Common shader functions"""
from kivy.graphics import opengl

"""Shader"""
class Shader(object):
    def __init__(self, *argv):
        self.program = opengl.glCreateProgram()
        #shader_bytes = self.read_shader(path)
        #vertex = opengl.glCreateShader(opengl.GL_VERTEX_SHADER)

        shader_ids = []
        for shader_path in argv:
            shader_ids.append(self.compile_shader(shader_path))

        for shader in shader_ids:
            opengl.glAttachShader(self.program, shader)

        opengl.glLinkProgram(self.program)

        for shader in shader_ids:
            opengl.glDetachShader(self.program, shader)

    def compile_shader(self, path):
        extension_map = {
            "vert": opengl.GL_VERTEX_SHADER,
            "frag": opengl.GL_FRAGMENT_SHADER,
        }
        return self.create_shader(path, extension_map[path.split('.')[1]])

    def create_shader(self, path, shader_type):
        shader_code = self.read_shader(path)
        shader = opengl.glCreateShader(shader_type)
        opengl.glShaderSource(shader, shader_code)
        opengl.glCompileShader(shader)
        self.check_err(shader)
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
        err = opengl.glGetShaderiv(shader, opengl.GL_COMPILE_STATUS)
        if err != opengl.GL_TRUE:
            print(opengl.glGetShaderInfoLog(shader, 1024).decode('ASCII'))

    def get_program(self):
        return self.program