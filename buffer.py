import numpy as np
from kivy.graphics import opengl

class Buffer(object):
    def __init__(self, *argv):
        (self.vbo,) = opengl.glGenBuffers(1)
        self.num_vertices = 0
        self.point_nbytes = 0
        self.normal_nbytes = 0
        self.tangent_nbytes = 0

        self.add_buffer_data(*argv)
        
    def add_buffer_data(self, *argv):
        opengl.glBindBuffer(opengl.GL_ARRAY_BUFFER, self.vbo)
        index = 0
        num_bytes = 0
        for data in argv:
            num_bytes += data.nbytes

        opengl.glBufferData(opengl.GL_ARRAY_BUFFER, num_bytes, bytes([0]), opengl.GL_STATIC_DRAW)
        byte_offset = 0
        for data in argv:
            if index == 0:
                self.point_nbytes
                self.num_vertices = len(data)
            if index == 1:
                self.normal_nbytes = data.nbytes
            if index == 2:
                self.tangent_nbytes = data.nbytes
            opengl.glBufferSubData(opengl.GL_ARRAY_BUFFER, byte_offset, data.nbytes, data.tobytes())
            byte_offset += data.nbytes
            index += 1

        #return self.vbo