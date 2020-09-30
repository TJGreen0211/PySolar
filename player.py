import numpy as np

from kivy.graphics import opengl
from PIL import Image

from shader import Shader

class Player(object):
    def __init__(self, buffer):
        self.buffer = buffer
        self.mesh_shader = Shader("shaders/object.vert", "shaders/object.frag").get_program()

    def draw(self, arcball_camera, perspective_arr, view_arr):
        opengl.glUseProgram(self.mesh_shader)
        opengl.glBindBuffer(opengl.GL_ARRAY_BUFFER, self.buffer.vbo)

        u_loc = opengl.glGetUniformLocation(self.mesh_shader, b"projection")
        opengl.glUniformMatrix4fv(u_loc, 1, False, np.array(perspective_arr).flatten().tobytes())
        view_loc = opengl.glGetUniformLocation(self.mesh_shader, b"view")
        opengl.glUniformMatrix4fv(view_loc, 1, False, view_arr.flatten().tobytes())
        model_loc = opengl.glGetUniformLocation(self.mesh_shader, b"model")
        
        model = np.identity(4, dtype=np.float32)
        model_position = arcball_camera.position
        model[0][3] = model_position[0]
        model[1][3] = model_position[1]
        model[2][3] = model_position[2]

        
        model = np.dot(model, np.array(arcball_camera.view_rotation, dtype=np.float32).reshape(4, 4))
        
        opengl.glUniformMatrix4fv(model_loc, 1, False, model.flatten().tobytes())

        camera_position = arcball_camera.camera_model_view_position(list(model.flatten()))
        opengl.glUniform3f(opengl.glGetUniformLocation(self.mesh_shader, b"camera_position"), camera_position[0], camera_position[1], camera_position[2])
        opengl.glUniform3f(opengl.glGetUniformLocation(self.mesh_shader, b"lightPosition"), 10.0, 5.0, -4.0)

        opengl.glEnableVertexAttribArray(0)
        opengl.glVertexAttribPointer(0, 3, opengl.GL_FLOAT, False, 12, 0)
        opengl.glEnableVertexAttribArray(1)
        opengl.glVertexAttribPointer(1, 3, opengl.GL_FLOAT, False, 12, self.buffer.point_nbytes)
        opengl.glDrawArrays(opengl.GL_TRIANGLES, 0, self.buffer.num_vertices)