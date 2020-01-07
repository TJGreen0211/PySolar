import os
import sys
import math
import ctypes

import kivy
kivy.require('1.1.1')
from kivy.app import App
from kivy.clock import Clock
from kivy.core.window import Window
from kivy.core.image import Image
from kivy.uix.widget import Widget
from kivy.uix.floatlayout import FloatLayout
from kivy.resources import resource_find
#from kivy.graphics.transformation import Matrix
from kivy.graphics import Fbo
from kivy.graphics import Rectangle

from kivy.graphics import opengl

import numpy as np

sys.path.insert(1, os.getcwd()+'/bin')
import waves
import geometry

vertex_code = b"""
	uniform mat4 u_projection_mat;
	attribute vec3 a_position;

	varying vec4 v_color;
	void main()
	{
		gl_Position = u_projection_mat*vec4(a_position, 1.0);
		v_color = (vec4(a_position, 1.0)+1.0)/2.0;
	} """



fragment_code = b"""
	varying vec4 v_color;
	void main()
	{
		gl_FragColor = v_color;
	} """


#def createShader(vertPath, fragPath, tchsPath=None, teshPath=None, geomPath=None):
#	vert = loadShaderString(vertPath)
#	vertId = loadShader(vert, GL_VERTEX_SHADER)
#
#	frag = loadShaderString(fragPath)
#	fragId = loadShader(frag, GL_FRAGMENT_SHADER)
#
#	return LinkShader(fragID, vertID)


data = np.array([ -0.5,-0.5,0.5,
					0.5,-0.5,0.5,
					0.5,0.5,0.5,
				   -0.5,0.5,0.5], dtype=np.float32)



index = np.array([0,1,2,  2,3,0],dtype=np.uint32)


class Application(Widget):
	def __init__(self, **kwargs):
		super(Application, self).__init__(**kwargs)
		self.angle=0
		self.setup_kvfbo()
		self.setup_glfbo()
		#self.dim = 128
		#self.dim2 = self.dim*self.dim
		#self.waves_lib = self.loadDLL(os.getcwd()+"/bin/libWaves.dll")
		#self.waves_lib.initializeWaveInterface(ctypes.c_int(self.dim))
		self.wavePatch = waves.Waves(dimension=128)


		Clock.schedule_interval(self.update_glsl, 1.0 / 60.0)

	def update_glsl(self, *largs):
		self.angle+=0.01
		#self.waves_lib.generateWavePatches(ctypes.c_double(self.angle))
		#self.waves_lib.getDisplacementdx.restype = ctypes.POINTER(ctypes.c_double * (self.dim2*4))
		#self.waves_lib.getDisplacementdy.restype = ctypes.POINTER(ctypes.c_double * (self.dim2*4))
		#self.waves_lib.getDisplacementdz.restype = ctypes.POINTER(ctypes.c_double * (self.dim2*4))

		self.wavePatch.generate_waves(self.angle)
		if 0:
			self.draw_fbo(self.kvfboid)
		else:
			self.draw_fbo(self.glfboid)
			self.blit_fbo()
		self.canvas.ask_update()

	def setup_kvfbo(self):
		self.kvfbo = Fbo(with_depthbuffer = True, size = Window.size, compute_normal_mat=True)
		self.canvas.add(self.kvfbo)
		self.canvas.add(Rectangle(size=Window.size, texture=self.kvfbo.texture))
		self.kvfbo.bind()
		(self.kvfboid,)=opengl.glGetIntegerv(opengl.GL_FRAMEBUFFER_BINDING)
		self.kvfbo.release()

	def setup_glfbo(self):
		self.program = opengl.glCreateProgram()

		vertex	 = opengl.glCreateShader(opengl.GL_VERTEX_SHADER)
		fragment = opengl.glCreateShader(opengl.GL_FRAGMENT_SHADER)
		opengl.glShaderSource(vertex, vertex_code)
		opengl.glShaderSource(fragment, fragment_code)
		opengl.glCompileShader(vertex)
		opengl.glCompileShader(fragment)

		opengl.glAttachShader(self.program, vertex)
		opengl.glAttachShader(self.program, fragment)
		opengl.glLinkProgram(self.program)
		opengl.glDetachShader(self.program, vertex)
		opengl.glDetachShader(self.program, fragment)
		opengl.glUseProgram(self.program)

		self.w, self.h = Window.width, Window.height
		opengl.glEnable(opengl.GL_TEXTURE_2D)
		(self.glfboid,) = opengl.glGenFramebuffers(1)
		(self.gltexid,) = opengl.glGenTextures(1)
		opengl.glBindFramebuffer(opengl.GL_FRAMEBUFFER, self.glfboid)
		opengl.glBindTexture(opengl.GL_TEXTURE_2D, self.gltexid)

		opengl.glTexImage2D(opengl.GL_TEXTURE_2D, 0, opengl.GL_RGBA, self.w, self.h, 0, opengl.GL_RGBA, opengl.GL_UNSIGNED_BYTE, (np.ones(self.w*self.h*4, np.uint8)*128).tobytes())
		opengl.glFramebufferTexture2D(opengl.GL_FRAMEBUFFER, opengl.GL_COLOR_ATTACHMENT0, opengl.GL_TEXTURE_2D,self.gltexid,0)
		opengl.glBindFramebuffer(opengl.GL_FRAMEBUFFER, 0)
		opengl.glBindTexture(opengl.GL_TEXTURE_2D, 0)

		g = geometry.Geometry(subdivisions=4)
		g.tetrahedron_sphere(4)
		#print(g.tetrahedron_divisions)
		#print(g.tetrahedron_points)
		(self.sphereVBO,) = opengl.glGenBuffers(1)
		opengl.glBindBuffer(opengl.GL_ARRAY_BUFFER, self.sphereVB)
		opengl.glBufferData(opengl.GL_ARRAY_BUFFER, g.point_byte_size + g.normal_byte_size, None, opengl.GL_STATIC_DRAW)
		opengl.glBufferSubData(opengl.GL_ARRAY_BUFFER, 0,	g.point_byte_size, bytearray(g.tetrahedron_points))
		opengl.glBufferSubData(opengl.GL_ARRAY_BUFFER, g.point_byte_size, g.normal_byte_size, g.tetrahedron_normals)

		#
		#
		(self.vbo,) = opengl.glGenBuffers(1)
		opengl.glBindBuffer(opengl.GL_ARRAY_BUFFER, self.vbo)
		opengl.glBufferData(opengl.GL_ARRAY_BUFFER, data.nbytes, data.tobytes(), opengl.GL_DYNAMIC_DRAW)
		(self.ibo,)= opengl.glGenBuffers(1)
		opengl.glBindBuffer(opengl.GL_ELEMENT_ARRAY_BUFFER, self.ibo)
		opengl.glBufferData(opengl.GL_ELEMENT_ARRAY_BUFFER, index.nbytes, index.tobytes(), opengl.GL_STATIC_DRAW)

	def blit_fbo(self):
		opengl.glBindFramebuffer(opengl.GL_FRAMEBUFFER, self.glfboid)
		pixels = opengl.glReadPixels(0, 0, self.w, self.h, opengl.GL_RGBA, opengl.GL_UNSIGNED_BYTE)
		opengl.glBindFramebuffer(opengl.GL_FRAMEBUFFER,self.kvfboid)
		opengl.glTexSubImage2D(opengl.GL_TEXTURE_2D, 0 ,0, 0, self.w, self.h, opengl.GL_RGBA, opengl.GL_UNSIGNED_BYTE, pixels)
		opengl.glBindFramebuffer(opengl.GL_FRAMEBUFFER,0)

	def draw_fbo(self, targetfbo):
		eye=np.matrix([[np.cos(self.angle),-np.sin(self.angle),0,0],
				[np.sin(self.angle),np.cos(self.angle),0,0],
				[0,0,1,0],
				[0,0,0,1]],dtype=np.float32)

		opengl.glBindFramebuffer(opengl.GL_FRAMEBUFFER,targetfbo)
		opengl.glClear(opengl.GL_COLOR_BUFFER_BIT)

		opengl.glBindBuffer(opengl.GL_ARRAY_BUFFER, self.vbo)
		opengl.glBindBuffer(opengl.GL_ELEMENT_ARRAY_BUFFER, self.ibo)

		opengl.glUseProgram(self.program)
		u_loc = opengl.glGetUniformLocation(self.program, b"u_projection_mat")
		opengl.glUniformMatrix4fv(u_loc, 1, False, np.array(eye).flatten().tobytes() )
		a_loc = opengl.glGetAttribLocation(self.program, b"a_position")
		opengl.glEnableVertexAttribArray(a_loc)
		opengl.glVertexAttribPointer(a_loc, 3, opengl.GL_FLOAT, False, 12, 0)

		opengl.glViewport(0,0,self.w,self.h)
		opengl.glDrawElements(opengl.GL_TRIANGLES, 6, opengl.GL_UNSIGNED_INT, 0)
		opengl.glBindFramebuffer(opengl.GL_FRAMEBUFFER, 0)

	def update(self, dt):
		#dx_texture = glGenTextures(1)
		#dy_texture = glGenTextures(1)
		#dz_texture = glGenTextures(1)

		#glViewport(0, 0, width, height)
		opengl.glEnable(opengl.GL_DEPTH_TEST)
		opengl.glClearColor(0.5, 0.5, 0.5,1.0)
		opengl.glClear(opengl.GL_COLOR_BUFFER_BIT | opengl.GL_DEPTH_BUFFER_BIT)

		#bindTextureAttachment(dx_texture, dx_texture_data, dim, dim)
		#bindTextureAttachment(dy_texture, dy_texture_data, dim, dim)
		#bindTextureAttachment(dz_texture, dz_texture_data, dim, dim)

		#print(dy_texture_data)

	def loadDLL(self, path):
		return ctypes.CDLL(path)

class MainApp(App):
	cwd = os.getcwd()
	os.chdir(cwd)

	def build(self):
		#application = ApplicationRun()
		#Clock.schedule_interval(application.update, 1.0 / 60.0)
		return Application()

def createBuffer(vertices, normals, texCoords):
	vao = opengl.glGenVertexArrays(1)
	opengl.glBindVertexArray(vao)
	vbo = opengl.glGenBuffers(1)
	opengl.glBindBuffer(opengl.GL_ARRAY_BUFFER, vbo)

	opengl.glBufferData(opengl.GL_ARRAY_BUFFER, opengl.ArrayDatatype.arrayByteCount(vertices)+opengl.ArrayDatatype.arrayByteCount(normals)+opengl.ArrayDatatype.arrayByteCount(texCoords), None, opengl.GL_STATIC_DRAW)
	opengl.glBufferSubData(opengl.GL_ARRAY_BUFFER, 0, opengl.ArrayDatatype.arrayByteCount(vertices), vertices)
	opengl.glBufferSubData(opengl.GL_ARRAY_BUFFER, opengl.ArrayDatatype.arrayByteCount(vertices), opengl.ArrayDatatype.arrayByteCount(normals), normals)
	opengl.glBufferSubData(opengl.GL_ARRAY_BUFFER, opengl.ArrayDatatype.arrayByteCount(vertices)+opengl.ArrayDatatype.arrayByteCount(normals), opengl.ArrayDatatype.arrayByteCount(texCoords), texCoords)

	opengl.glVertexAttribPointer(0, 3, opengl.GL_FLOAT, opengl.GL_FALSE, opengl.ArrayDatatype.ArrayByteCount(vertices[0]), ctypes.c_void_p(0))
	opengl.glEnableVertexAttribArray(0)
	opengl.glVertexAttribPointer(1, 3, opengl.GL_FLOAT, opengl.GL_FALSE, opengl.ArrayDatatype.ArrayByteCount(normals[0]), ctypes.c_void_p(vertices.itemsize))
	opengl.glEnableVertexAttribArray(1)
	opengl.glVertexAttribPointer(2, 2, opengl.GL_FLOAT, opengl.GL_FALSE, opengl.ArrayDatatype.ArrayByteCount(texCoords[0]), ctypes.c_void_p(vertices.itemsize+texCoords.itemsize))
	opengl.glEnableVertexAttribArray(2)

	opengl.glBindBuffer(opengl.GL_ARRAY_BUFFER, 0)
	opengl.glBindVertexArray(0)

	return vao

"""def drawObject(shader, vao, position, texture, vertices, m, v, p, delta):
	opengl.glUseProgram(shader)
	opengl.glBindVertexArray(vao)
	camPosition = getCameraPosition(translate(position))

	opengl.glUniformMatrix4fv(opengl.glGetUniformLocation(shader, "model"), 1, opengl.GL_FALSE, m)
	opengl.glUniformMatrix4fv(opengl.glGetUniformLocation(shader, "view"), 1, opengl.GL_FALSE, v)
	opengl.glUniformMatrix4fv(opengl.glGetUniformLocation(shader, "projection"), 1, opengl.GL_FALSE, p)

	opengl.glUniform3f(opengl.glGetUniformLocation(shader, "cameraPosition"), camPosition[0], camPosition[1], camPosition[2])
	opengl.glUniform3f(opengl.glGetUniformLocation(shader, "lightPosition"), 0.0, 0.0, -10.0)

	opengl.glActiveTexture(opengl.GL_TEXTURE0)
	opengl.glBindTexture(opengl.GL_TEXTURE_2D, texture)
	opengl.glUniform1i(opengl.glGetUniformLocation(shader, "texture1"), 0)

	opengl.glDrawArrays(opengl.GL_TRIANGLES, 0, vertices)

	opengl.glBindTexture(opengl.GL_TEXTURE_2D, 0)
	opengl.glUseProgram(0)
	opengl.glBindVertexArray(0)"""

def onMouseButton():
	pass

def loadDLL(path):
	return ctypes.CDLL(path)

def bindTextureAttachment(texture_id, texture_data, width, height):
	opengl.glBindTexture(opengl.GL_TEXTURE_2D, texture_id)
	opengl.glTexImage2D(opengl.GL_TEXTURE_2D, 0, opengl.GL_RGBA, width, height, 0, opengl.GL_RGBA, opengl.GL_FLOAT, texture_data)
	opengl.glTexParameteri( opengl.GL_TEXTURE_2D, opengl.GL_TEXTURE_MIN_FILTER, opengl.GL_NEAREST )
	opengl.glTexParameteri( opengl.GL_TEXTURE_2D, opengl.GL_TEXTURE_MAG_FILTER, opengl.GL_NEAREST )
	opengl.glTexParameteri(opengl.GL_TEXTURE_2D, opengl.GL_TEXTURE_WRAP_S, opengl.GL_REPEAT)
	opengl.glTexParameteri(opengl.GL_TEXTURE_2D, opengl.GL_TEXTURE_WRAP_T, opengl.GL_REPEAT)
	opengl.glBindTexture(opengl.GL_TEXTURE_2D, 0)

def main():

	#program = createShader("shader.vert", "shader.frag")

	width, height = 640, 480
	aspect = width/float(height)

	opengl.glViewport(0, 0, width, height)
	opengl.glEnable(opengl.GL_DEPTH_TEST)
	opengl.glClearColor(0.5, 0.5, 0.5,1.0)


	opengl.glClear(opengl.GL_COLOR_BUFFER_BIT | opengl.GL_DEPTH_BUFFER_BIT)
	#waves_lib.destroyWaves()
	print("Program terminated")

def test():
	#test_lib = ctypes.CDLL("C:/Users/TJ/Documents/programming/python/PyGL/test.dll")
	mat_math = loadDLL(os.getcwd()+"/bin/matrixMath.dll")

	out = mat_math.mat4RotateX(ctypes.c_double(1.0))
	print(out)

if __name__ == "__main__":
	#test()
	MainApp().run()

