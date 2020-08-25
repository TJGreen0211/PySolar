import numpy as np
from kivy.graphics import opengl

class Buffer(object):
    def __init__(self):
        (self.vbo,) = opengl.glGenBuffers(1)
        
    def add_buffer_data(self, *argv):
        opengl.glBindBuffer(opengl.GL_ARRAY_BUFFER, self.vbo)
        num_bytes = 0
        for data in argv:
            num_bytes += data.nbytes

        opengl.glBufferData(opengl.GL_ARRAY_BUFFER, num_bytes, bytes([0]), opengl.GL_STATIC_DRAW)
        byte_offset = 0
        for data in argv:
            opengl.glBufferSubData(opengl.GL_ARRAY_BUFFER, byte_offset, data.nbytes, data.tobytes())
            byte_offset += data.nbytes

        return self.vbo