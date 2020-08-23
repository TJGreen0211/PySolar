import OpenGL
from kivy.graphics.opengl import *

def loadShader(shaderString, shaderType):
	try:
		shaderID = glCreateShader(shaderType)
		glShaderSource(shaderID, shaderString)
		glCompileShader(shaderID)
		if glGetShaderiv(shaderID, GL_COMPILE_STATUS) != True:
			info = glGetShaderInfoLog(shaderID)
			raise RuntimeError('Shader compilation failed %s' %(info))
		return shaderID
	except:
		glDeleteShader(shaderID)
		raise
		
def linkShader(vertID, fragID):
	programID = glCreateProgram()
	
	glAttachShader(programID, vertID)
	glAttachShader(programID, fragID)
	glLinkProgram(programID)
	
	if glGetProgramiv(programID, GL_LINK_STATUS) != True:
		info = glGetProgramInfoLog(programID)
		glDeleteProgram(programID)
		glDeleteShader(vertID)
		glDeleteShader(fragID)
		raise RuntimeError('Error linking program %s' %(info))
	glDeleteShader(vertID)
	glDeleteShader(fragID)
	
	return programID

def loadShaderString(path):
	shaderString = ''
	with open(path, 'r') as shaderFile:
		shaderString = shaderFile.read()
	return shaderString
	
def createShader(vertPath, fragPath, tcshPath=None, teshPath=None, geomPath=None):
	vert = loadShaderString(vertPath)
	vertId = loadShader(vert, GL_VERTEX_SHADER)
	
	frag = loadShaderString(fragPath)
	fragId = loadShader(frag, GL_FRAGMENT_SHADER)
	
	if tcshPath is not None:
		vert = loadShaderString(vertPath)
		vertId = loadShader(vert, GL_TESS_CONTROL_SHADER)
	
	if tcshPath is not None:
		vert = loadShaderString(vertPath)
		vertId = loadShader(vert, GL_TESS_EVALUATION_SHADER)
	
	if tcshPath is not None:
		vert = loadShaderString(vertPath)
		vertId = loadShader(vert, GL_GEOMETRY_SHADER)
		
	return LinkShader(fragID, vertID)