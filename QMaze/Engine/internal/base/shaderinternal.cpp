#include "shaderinternal.h"

#include "tools/debug.h"
#include "tools/string.h"
#include "internal/misc/loader.h"
#include "internal/base/renderstate.h"

ShaderDescription ShaderInternal::descriptions_[] =  {
	GL_VERTEX_SHADER, "VertexShader", "vert",
	GL_TESS_CONTROL_SHADER, "TessellationControlShader", "tesc",
	GL_TESS_EVALUATION_SHADER, "TessellationEvaluationShader", "tese",
	GL_GEOMETRY_SHADER, "GeometryShader", "geom",
	GL_FRAGMENT_SHADER, "FragmentShader", "frag"
};

ShaderInternal::ShaderInternal() : ObjectInternal(ObjectShader) {
	program_ = glCreateProgram();
	std::fill(shaderObjs_, shaderObjs_ + ShaderTypeCount, 0);
}

ShaderInternal::~ShaderInternal() {
	glDeleteProgram(program_);
	ClearIntermediateShaders();
}

bool ShaderInternal::Load(const std::string& path) {
	ShaderParser parser;
	std::string sources[ShaderTypeCount];
	if (!parser.Parse(path, sources)) {
		return false;
	}

	for (int i = 0; i < ShaderTypeCount; ++i) {
		if (!sources[i].empty() && !LoadSource((ShaderType)i, sources[i].c_str())) {
			return false;
		}
	}

	return Link();
}

bool ShaderInternal::GetErrorMessage(GLuint shaderObj, std::string& answer) {
	if (shaderObj == 0) {
		answer = "invalid shader id";
		return false;
	}

	GLint length = 0, writen = 0;
	glGetShaderiv(shaderObj, GL_INFO_LOG_LENGTH, &length);
	if (length > 1) {
		char* log = new char[length];
		glGetShaderInfoLog(shaderObj, length, &writen, log);
		answer = log;
		delete[] log;
		return true;
	}

	return false;
}

GLuint ShaderInternal::GenerateBindingIndex() const {
	static GLuint bindingIndex = 1;
	return bindingIndex++;
}

bool ShaderInternal::Link() {
	glLinkProgram(program_);
	
	GLint status = GL_FALSE;
	glGetProgramiv(program_, GL_LINK_STATUS, &status);

	if (status != GL_TRUE) {
		Debug::LogError("failed to link shader.");
		return false;
	}

	glValidateProgram(program_);
	glGetProgramiv(program_, GL_VALIDATE_STATUS, &status);
	if (status != GL_TRUE) {
		Debug::LogError("failed to validate shader.");
		return false;
	}

	ClearIntermediateShaders();

	return true;
}

void ShaderInternal::ClearIntermediateShaders() {
	for (int i = 0; i < ShaderTypeCount; ++i) {
		if (shaderObjs_[i] != 0) {
			glDeleteShader(shaderObjs_[i]);
			shaderObjs_[i] = 0;
		}
	}
}

bool ShaderInternal::LoadSource(ShaderType shaderType, const char* source) {
	GLuint shaderObj = glCreateShader(Description(shaderType).glShaderType);

	glShaderSource(shaderObj, 1, &source, nullptr);
	glCompileShader(shaderObj);

	glAttachShader(program_, shaderObj);
	
	std::string message;
	if (!GetErrorMessage(shaderObj, message)) {
		if (shaderObjs_[shaderType] != 0) {
			glDeleteShader(shaderObjs_[shaderType]);
		}
		shaderObjs_[shaderType] = shaderObj;
		return true;
	}

	AssertX(false, Description(shaderType).name + std::string(" ") + message);
	return false;
}

void ShaderInternal::SetUniform(const std::string& name, int value) {
	AssertX(uniforms_.contains(name), "invalid uniform " + name + ".");
	Uniform* u = uniforms_[name];
	glProgramUniform1i(program_, u->location, value);
}

void ShaderInternal::SetUniform(const std::string& name, float value) {
	AssertX(uniforms_.contains(name), "invalid uniform " + name + ".");
	Uniform* u = uniforms_[name];
	glProgramUniform1f(program_, u->location, value);
}

void ShaderInternal::SetUniform(const std::string& name, const void* value) {
	AssertX(uniforms_.contains(name), "invalid uniform " + name + ".");
	Uniform* u = uniforms_[name];
	switch (u->type) {

		// Floats
	case GL_FLOAT:
		glProgramUniform1fv(program_, u->location, u->size, (const GLfloat *)value);
		break;
	case GL_FLOAT_VEC2:
		glProgramUniform2fv(program_, u->location, u->size, (const GLfloat *)value);
		break;
	case GL_FLOAT_VEC3:
		glProgramUniform3fv(program_, u->location, u->size, (const GLfloat *)value);
		break;
	case GL_FLOAT_VEC4:
		glProgramUniform4fv(program_, u->location, u->size, (const GLfloat *)value);
		break;

		// Doubles
	case GL_DOUBLE:
		glProgramUniform1dv(program_, u->location, u->size, (const GLdouble *)value);
		break;
	case GL_DOUBLE_VEC2:
		glProgramUniform2dv(program_, u->location, u->size, (const GLdouble *)value);
		break;
	case GL_DOUBLE_VEC3:
		glProgramUniform3dv(program_, u->location, u->size, (const GLdouble *)value);
		break;
	case GL_DOUBLE_VEC4:
		glProgramUniform4dv(program_, u->location, u->size, (const GLdouble *)value);
		break;

		// Samplers, Ints and Bools
	case GL_SAMPLER_1D:
	case GL_SAMPLER_2D:
	case GL_SAMPLER_3D:
	case GL_SAMPLER_CUBE:
	case GL_SAMPLER_1D_SHADOW:
	case GL_SAMPLER_2D_SHADOW:
	case GL_SAMPLER_1D_ARRAY:
	case GL_SAMPLER_2D_ARRAY:
	case GL_SAMPLER_1D_ARRAY_SHADOW:
	case GL_SAMPLER_2D_ARRAY_SHADOW:
	case GL_SAMPLER_2D_MULTISAMPLE:
	case GL_SAMPLER_2D_MULTISAMPLE_ARRAY:
	case GL_SAMPLER_CUBE_SHADOW:
	case GL_SAMPLER_BUFFER:
	case GL_SAMPLER_2D_RECT:
	case GL_SAMPLER_2D_RECT_SHADOW:
	case GL_INT_SAMPLER_1D:
	case GL_INT_SAMPLER_2D:
	case GL_INT_SAMPLER_3D:
	case GL_INT_SAMPLER_CUBE:
	case GL_INT_SAMPLER_1D_ARRAY:
	case GL_INT_SAMPLER_2D_ARRAY:
	case GL_INT_SAMPLER_2D_MULTISAMPLE:
	case GL_INT_SAMPLER_2D_MULTISAMPLE_ARRAY:
	case GL_INT_SAMPLER_BUFFER:
	case GL_INT_SAMPLER_2D_RECT:
	case GL_UNSIGNED_INT_SAMPLER_1D:
	case GL_UNSIGNED_INT_SAMPLER_2D:
	case GL_UNSIGNED_INT_SAMPLER_3D:
	case GL_UNSIGNED_INT_SAMPLER_CUBE:
	case GL_UNSIGNED_INT_SAMPLER_1D_ARRAY:
	case GL_UNSIGNED_INT_SAMPLER_2D_ARRAY:
	case GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE:
	case GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE_ARRAY:
	case GL_UNSIGNED_INT_SAMPLER_BUFFER:
	case GL_UNSIGNED_INT_SAMPLER_2D_RECT:
	case GL_BOOL:
	case GL_INT:
		glProgramUniform1iv(program_, u->location, u->size, (const GLint *)value);
		break;
	case GL_BOOL_VEC2:
	case GL_INT_VEC2:
		glProgramUniform2iv(program_, u->location, u->size, (const GLint *)value);
		break;
	case GL_BOOL_VEC3:
	case GL_INT_VEC3:
		glProgramUniform3iv(program_, u->location, u->size, (const GLint *)value);
		break;
	case GL_BOOL_VEC4:
	case GL_INT_VEC4:
		glProgramUniform4iv(program_, u->location, u->size, (const GLint *)value);
		break;

		// Unsigned ints
	case GL_UNSIGNED_INT:
		glProgramUniform1uiv(program_, u->location, u->size, (const GLuint *)value);
		break;
	case GL_UNSIGNED_INT_VEC2:
		glProgramUniform2uiv(program_, u->location, u->size, (const GLuint *)value);
		break;
	case GL_UNSIGNED_INT_VEC3:
		glProgramUniform3uiv(program_, u->location, u->size, (const GLuint *)value);
		break;
	case GL_UNSIGNED_INT_VEC4:
		glProgramUniform4uiv(program_, u->location, u->size, (const GLuint *)value);
		break;

		// Float Matrices
	case GL_FLOAT_MAT2:
		glProgramUniformMatrix2fv(program_, u->location, u->size, false, (const GLfloat *)value);
		break;
	case GL_FLOAT_MAT3:
		glProgramUniformMatrix3fv(program_, u->location, u->size, false, (const GLfloat *)value);
		break;
	case GL_FLOAT_MAT4:
		glProgramUniformMatrix4fv(program_, u->location, u->size, false, (const GLfloat *)value);
		break;
	case GL_FLOAT_MAT2x3:
		glProgramUniformMatrix2x3fv(program_, u->location, u->size, false, (const GLfloat *)value);
		break;
	case GL_FLOAT_MAT2x4:
		glProgramUniformMatrix2x4fv(program_, u->location, u->size, false, (const GLfloat *)value);
		break;
	case GL_FLOAT_MAT3x2:
		glProgramUniformMatrix3x2fv(program_, u->location, u->size, false, (const GLfloat *)value);
		break;
	case GL_FLOAT_MAT3x4:
		glProgramUniformMatrix3x4fv(program_, u->location, u->size, false, (const GLfloat *)value);
		break;
	case GL_FLOAT_MAT4x2:
		glProgramUniformMatrix4x2fv(program_, u->location, u->size, false, (const GLfloat *)value);
		break;
	case GL_FLOAT_MAT4x3:
		glProgramUniformMatrix4x3fv(program_, u->location, u->size, false, (const GLfloat *)value);
		break;

		// Double Matrices
	case GL_DOUBLE_MAT2:
		glProgramUniformMatrix2dv(program_, u->location, u->size, false, (const GLdouble *)value);
		break;
	case GL_DOUBLE_MAT3:
		glProgramUniformMatrix3dv(program_, u->location, u->size, false, (const GLdouble *)value);
		break;
	case GL_DOUBLE_MAT4:
		glProgramUniformMatrix4dv(program_, u->location, u->size, false, (const GLdouble *)value);
		break;
	case GL_DOUBLE_MAT2x3:
		glProgramUniformMatrix2x3dv(program_, u->location, u->size, false, (const GLdouble *)value);
		break;
	case GL_DOUBLE_MAT2x4:
		glProgramUniformMatrix2x4dv(program_, u->location, u->size, false, (const GLdouble *)value);
		break;
	case GL_DOUBLE_MAT3x2:
		glProgramUniformMatrix3x2dv(program_, u->location, u->size, false, (const GLdouble *)value);
		break;
	case GL_DOUBLE_MAT3x4:
		glProgramUniformMatrix3x4dv(program_, u->location, u->size, false, (const GLdouble *)value);
		break;
	case GL_DOUBLE_MAT4x2:
		glProgramUniformMatrix4x2dv(program_, u->location, u->size, false, (const GLdouble *)value);
		break;
	case GL_DOUBLE_MAT4x3:
		glProgramUniformMatrix4x3dv(program_, u->location, u->size, false, (const GLdouble *)value);
		break;
	}
}

void ShaderInternal::SetBlock(const std::string& name, const void* value) {
	AssertX(blocks_.contains(name), "invalid block name " + name + ".");
	UniformBlock* block = blocks_[name];
	glBindBuffer(GL_UNIFORM_BUFFER, block->buffer);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, block->size, value);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void ShaderInternal::SetBlockUniform(const std::string& blockName, const std::string& uniformName, const void* value) {
	AssertX(blocks_.contains(blockName), "invalid block name " + blockName + ".");
	UniformBlock* block = blocks_[blockName];

	std::string composedUniformName = blockName + "." + uniformName;
	std::string finalUniformName;

	if (block->uniforms.contains(uniformName)) {
		finalUniformName = uniformName;
	}
	else if(block->uniforms.contains(composedUniformName)) {
		finalUniformName = composedUniformName;
	}

	AssertX(!finalUniformName.empty(), "invalid uniform name " + uniformName + ".");
	Uniform* uniform = block->uniforms[finalUniformName];
	glBindBuffer(GL_UNIFORM_BUFFER, block->buffer);
	glBufferSubData(GL_UNIFORM_BUFFER, uniform->offset, uniform->size, value);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void ShaderInternal::SetBlockUniformArrayElement(const std::string& blockName, const std::string& uniformName, GLint index, const void* value) {
	AssertX(blocks_.contains(blockName), "invalid block name " + blockName + ".");
	UniformBlock* block = blocks_[blockName];
	AssertX(block->uniforms.contains(uniformName), "invalid uniform name " + uniformName + ".");
	Uniform* uniform = block->uniforms[uniformName];

	glBindBuffer(GL_UNIFORM_BUFFER, block->buffer);
	glBufferSubData(GL_UNIFORM_BUFFER, uniform->offset + uniform->stride * index, uniform->stride, value);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

ShaderInternal::UniformBlock::UniformBlock() : buffer(0) {
}

ShaderInternal::UniformBlock::~UniformBlock() {
	if (buffer != 0) {
		glDeleteBuffers(1, &buffer);
	}
}
