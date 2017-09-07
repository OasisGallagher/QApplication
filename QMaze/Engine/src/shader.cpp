#include "debug.h"
#include "loader.h"
#include "shader.h"
#include "utilities.h"
#include "renderstate.h"

struct ShaderDescription {
	GLenum glShaderType;
	const char* name;
	const char* tag;
};

static const ShaderDescription descriptions[] = {
	GL_VERTEX_SHADER, "VertexShader", "vert",
	GL_TESS_CONTROL_SHADER, "TessellationControlShader", "tesc",
	GL_TESS_EVALUATION_SHADER, "TessellationEvaluationShader", "tese",
	GL_GEOMETRY_SHADER, "GeometryShader", "geom",
	GL_FRAGMENT_SHADER, "FragmentShader", "frag"
};

static const char* SHADER = "shader";
static const char* INCLUDE = "include";

ShaderPrivate::ShaderPrivate() : Object(ObjectShader) {
	program_ = glCreateProgram();
	std::fill(shaderObjs_, shaderObjs_ + ShaderTypeCount, 0);
}

ShaderPrivate::~ShaderPrivate() {
	glDeleteProgram(program_);
	ClearShaders();
}

bool ShaderPrivate::Load(const std::string& path) {
	ShaderXLoader loader;
	std::string sources[ShaderTypeCount];
	if (!loader.Load(path, sources)) {
		return false;
	}

	for (int i = 0; i < ShaderTypeCount; ++i) {
		if (!sources[i].empty() && !LoadShader((ShaderType)i, sources[i].c_str())) {
			return false;
		}
	}

	return true;
}

bool ShaderPrivate::Load(ShaderType shaderType, const std::string& path) {
	std::string source;
	if (!TextLoader::Load(path, source)) {
		return false;
	}

	return LoadShader(shaderType, source.c_str());
}

bool ShaderPrivate::Link() {
	for (int i = 0; i < ShaderTypeCount; ++i) {
		if (i == ShaderTypeVertex || i == ShaderTypeFragment) {
			AssertX(shaderObjs_[i], Utility::Format("invalid %s.", descriptions[i].name));
		}
	}

	return LinkShader();
}

bool ShaderPrivate::Bind() {
	RenderState::PushProgram(program_);
	return true;
}

void ShaderPrivate::Unbind() {
	RenderState::PopProgram();
}

bool ShaderPrivate::GetErrorMessage(GLuint shaderObj, std::string& answer) {
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

GLuint ShaderPrivate::GenerateBindingIndex() const {
	static GLuint bindingIndex = 1;
	return bindingIndex++;
}

void ShaderPrivate::AddAllBlocks() {
	blocks_.clear();

	GLsizei nameWritten;
	GLint blockCount, uniformCount, blockNameLength, uniformNameLength, dataSize;
	GLint uniformType, uniformSize, uniformOffset, uniformMatrixStride, uniformArrayStride;

	GLuint buffer;
	std::string blockName, uniformName;

	glGetProgramiv(program_, GL_ACTIVE_UNIFORM_BLOCKS, &blockCount);
	for (GLint i = 0; i < blockCount; ++i) {
		glGetActiveUniformBlockiv(program_, i, GL_UNIFORM_BLOCK_NAME_LENGTH, &blockNameLength);

		blockName.resize(blockNameLength);
		glGetActiveUniformBlockName(program_, i, blockNameLength, &nameWritten, &blockName[0]);
		blockName.resize(nameWritten);

		if (blocks_.contains(blockName)) {
			glUniformBlockBinding(program_, i, blocks_[blockName]->binding);
			continue;
		}

		UniformBlock* block = blocks_[blockName];

		glGetActiveUniformBlockiv(program_, i, GL_UNIFORM_BLOCK_DATA_SIZE, &dataSize);

		glGenBuffers(1, &buffer);
		glBindBuffer(GL_UNIFORM_BUFFER, buffer);
		glBufferData(GL_UNIFORM_BUFFER, dataSize, NULL, GL_DYNAMIC_DRAW);

		GLuint bindingIndex = GenerateBindingIndex();
		glUniformBlockBinding(program_, i, bindingIndex);
		glBindBufferRange(GL_UNIFORM_BUFFER, bindingIndex, buffer, 0, dataSize);

		glGetActiveUniformBlockiv(program_, i, GL_UNIFORM_BLOCK_ACTIVE_UNIFORMS, &uniformCount);

		GLuint* indices = new GLuint[uniformCount];
		glGetActiveUniformBlockiv(program_, i, GL_UNIFORM_BLOCK_ACTIVE_UNIFORM_INDICES, (GLint*)indices);

		glGetProgramiv(program_, GL_ACTIVE_UNIFORM_MAX_LENGTH, &uniformNameLength);
		uniformName.resize(uniformNameLength);

		for (GLint j = 0; j < uniformCount; ++j) {
			glGetActiveUniformName(program_, indices[j], uniformNameLength, &nameWritten, &uniformName[0]);
			uniformName.resize(nameWritten);

			Uniform* uniform = block->uniforms[uniformName];

			// for uniform Matrices { mat4 m };
			// uniformSize = 1, uniformMatrixStride = 16, uniformArrayStride = 0.
			// for uniform Matrices { mat4 m[2] }:
			// uniformSize = 2, uniformMatrixStride = 16, uniformArrayStride = 64.
			// for uniform Matrices { float m[4] }:
			// uniformSize = 4, uniformMatrixStride = 0, uniformArrayStride = 4.
			glGetActiveUniformsiv(program_, 1, &indices[j], GL_UNIFORM_TYPE, &uniformType);
			glGetActiveUniformsiv(program_, 1, &indices[j], GL_UNIFORM_SIZE, &uniformSize);
			glGetActiveUniformsiv(program_, 1, &indices[j], GL_UNIFORM_OFFSET, &uniformOffset);
			glGetActiveUniformsiv(program_, 1, &indices[j], GL_UNIFORM_MATRIX_STRIDE, &uniformMatrixStride);
			glGetActiveUniformsiv(program_, 1, &indices[j], GL_UNIFORM_ARRAY_STRIDE, &uniformArrayStride);

			uniform->offset = uniformOffset;
			uniform->type = uniformType;
			uniform->size = GetUniformSize(uniformType, uniformSize, uniformOffset, uniformMatrixStride, uniformArrayStride);
			uniform->stride = uniformArrayStride;
		}

		block->size = dataSize;
		block->binding = bindingIndex++;
		block->buffer = buffer;
	}
}

void ShaderPrivate::AddAllUniforms() {
	uniforms_.clear();

	GLenum type;
	GLuint location = 0;
	GLint size, count, maxLength, length, stride;

	glGetProgramiv(program_, GL_ACTIVE_UNIFORMS, &count);
	glGetProgramiv(program_, GL_ACTIVE_UNIFORM_MAX_LENGTH, &maxLength);

	char* name = new char[maxLength];
	for (int i = 0; i < count; ++i) {
		glGetActiveUniform(program_, i, maxLength, &length, &size, &type, name);

		location = glGetUniformLocation(program_, name);

		// -1 indicates that is not an active uniform, although it may be present in a
		// uniform block.
		if (location == GL_INVALID_INDEX) {
			continue;
		}

		stride = -1;

		// 当shader中有uniform array时, 报GL_INVALID_OPERATION.
		//glGetActiveUniformsiv(program_, 1, &location, GL_UNIFORM_ARRAY_STRIDE, &stride);

		Uniform* uniform = uniforms_[name];
		uniform->type = type;
		uniform->location = location;
		uniform->size = size;
		uniform->stride = stride;
	}

	delete[] name;
}

bool ShaderPrivate::LinkShader() {
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

	// http://www.lighthouse3d.com/tutorials/glsl-tutorial/uniform-blocks/
	AddAllUniforms();
	AddAllBlocks();

	ClearShaders();

	return true;
}

void ShaderPrivate::ClearShaders() {
	for (int i = 0; i < ShaderTypeCount; ++i) {
		if (shaderObjs_[i] != 0) {
			glDeleteShader(shaderObjs_[i]);
			shaderObjs_[i] = 0;
		}
	}
}

bool ShaderPrivate::LoadShader(ShaderType shaderType, const char* source) {
	GLuint shaderObj = glCreateShader(descriptions[shaderType].glShaderType);

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

	AssertX(false, descriptions[shaderType].name + std::string(" ") + message);
	return false;
}

void ShaderPrivate::SetUniform(const std::string& name, int value) {
	AssertX(uniforms_.contains(name), "invalid uniform " + name + ".");
	Uniform* u = uniforms_[name];
	glProgramUniform1i(program_, u->location, value);
}

void ShaderPrivate::SetUniform(const std::string& name, float value) {
	AssertX(uniforms_.contains(name), "invalid uniform " + name + ".");
	Uniform* u = uniforms_[name];
	glProgramUniform1f(program_, u->location, value);
}

void ShaderPrivate::SetUniform(const std::string& name, const void* value) {
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

void ShaderPrivate::SetBlock(const std::string& name, const void* value) {
	AssertX(blocks_.contains(name), "invalid block name " + name + ".");
	UniformBlock* block = blocks_[name];
	glBindBuffer(GL_UNIFORM_BUFFER, block->buffer);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, block->size, value);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void ShaderPrivate::SetBlockUniform(const std::string& blockName, const std::string& uniformName, const void* value) {
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

void ShaderPrivate::SetBlockUniformArrayElement(const std::string& blockName, const std::string& uniformName, GLint index, const void* value) {
	AssertX(blocks_.contains(blockName), "invalid block name " + blockName + ".");
	UniformBlock* block = blocks_[blockName];
	AssertX(block->uniforms.contains(uniformName), "invalid uniform name " + uniformName + ".");
	Uniform* uniform = block->uniforms[uniformName];

	glBindBuffer(GL_UNIFORM_BUFFER, block->buffer);
	glBufferSubData(GL_UNIFORM_BUFFER, uniform->offset + uniform->stride * index, uniform->stride, value);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

GLuint ShaderPrivate::GetUniformSize(GLint uniformType, GLint uniformSize, 
	GLint uniformOffset, GLint uniformMatrixStride, GLint uniformArrayStride) {
	if (uniformArrayStride > 0) {
		return uniformSize * uniformArrayStride;
	}

	if (uniformMatrixStride > 0) {
		switch (uniformType) {
		case GL_FLOAT_MAT2:
		case GL_FLOAT_MAT2x3:
		case GL_FLOAT_MAT2x4:
		case GL_DOUBLE_MAT2:
		case GL_DOUBLE_MAT2x3:
		case GL_DOUBLE_MAT2x4:
			return 2 * uniformMatrixStride;

		case GL_FLOAT_MAT3:
		case GL_FLOAT_MAT3x2:
		case GL_FLOAT_MAT3x4:
		case GL_DOUBLE_MAT3:
		case GL_DOUBLE_MAT3x2:
		case GL_DOUBLE_MAT3x4:
			return 3 * uniformMatrixStride;

		case GL_FLOAT_MAT4:
		case GL_FLOAT_MAT4x2:
		case GL_FLOAT_MAT4x3:
		case GL_DOUBLE_MAT4:
		case GL_DOUBLE_MAT4x2:
		case GL_DOUBLE_MAT4x3:
			return 4 * uniformMatrixStride;

		default:
			Debug::LogError("invalid uniformType " + std::to_string(uniformType) + ".");
			return 0;
		}
	}

	return GetSizeOfType(uniformType);
}

GLuint ShaderPrivate::GetSizeOfType(GLint type) {
	switch (type) {
	case GL_FLOAT:
		return sizeof(float);

	case GL_FLOAT_VEC2:
		return sizeof(float) * 2;

	case GL_FLOAT_VEC3:
		return sizeof(float) * 3;

	case GL_FLOAT_VEC4:
		return sizeof(float) * 4;

		// Doubles
	case GL_DOUBLE:
		return sizeof(double);

	case GL_DOUBLE_VEC2:
		return sizeof(double) * 2;

	case GL_DOUBLE_VEC3:
		return sizeof(double) * 3;

	case GL_DOUBLE_VEC4:
		return sizeof(double) * 4;

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
		return sizeof(int);

	case GL_BOOL_VEC2:
	case GL_INT_VEC2:
		return sizeof(int) * 2;

	case GL_BOOL_VEC3:
	case GL_INT_VEC3:
		return sizeof(int) * 3;

	case GL_BOOL_VEC4:
	case GL_INT_VEC4:
		return sizeof(int) * 4;

		// Unsigned ints
	case GL_UNSIGNED_INT:
		return sizeof(unsigned int);

	case GL_UNSIGNED_INT_VEC2:
		return sizeof(unsigned int) * 2;

	case GL_UNSIGNED_INT_VEC3:
		return sizeof(unsigned int) * 3;

	case GL_UNSIGNED_INT_VEC4:
		return sizeof(unsigned int) * 4;

		// Float Matrices
	case GL_FLOAT_MAT2:
		return sizeof(float) * 4;

	case GL_FLOAT_MAT3:
		return sizeof(float) * 9;

	case GL_FLOAT_MAT4:
		return sizeof(float) * 16;

	case GL_FLOAT_MAT2x3:
		return sizeof(float) * 6;

	case GL_FLOAT_MAT2x4:
		return sizeof(float) * 8;

	case GL_FLOAT_MAT3x2:
		return sizeof(float) * 6;

	case GL_FLOAT_MAT3x4:
		return sizeof(float) * 12;

	case GL_FLOAT_MAT4x2:
		return sizeof(float) * 8;

	case GL_FLOAT_MAT4x3:
		return sizeof(float) * 12;

		// Double Matrices
	case GL_DOUBLE_MAT2:
		return sizeof(double) * 4;

	case GL_DOUBLE_MAT3:
		return sizeof(double) * 9;

	case GL_DOUBLE_MAT4:
		return sizeof(double) * 16;

	case GL_DOUBLE_MAT2x3:
		return sizeof(double) * 6;

	case GL_DOUBLE_MAT2x4:
		return sizeof(double) * 8;

	case GL_DOUBLE_MAT3x2:
		return sizeof(double) * 6;

	case GL_DOUBLE_MAT3x4:
		return sizeof(double) * 12;

	case GL_DOUBLE_MAT4x2:
		return sizeof(double) * 8;

	case GL_DOUBLE_MAT4x3:
		return sizeof(double) * 12;

	default:
		Debug::LogError("invalid uniform type " + std::to_string(type) + ".");
		return 0;
	}

	return 0;
}

ShaderPrivate::UniformBlock::UniformBlock() : buffer(0) {
}

ShaderPrivate::UniformBlock::~UniformBlock() {
	if (buffer != 0) {
		glDeleteBuffers(1, &buffer);
	}
}


bool ShaderXLoader::Load(const std::string& path, std::string* answer) {
	std::vector<std::string> lines;
	if (!TextLoader::Load("resources/" + path, lines)) {
		return false;
	}

	Clear();

	answer_ = answer;
	return ParseShaderSource(lines);
}

void ShaderXLoader::Clear() {
	type_ = ShaderTypeCount;
	source_.clear();
	globals_.clear();
	answer_ = nullptr;
}

bool ShaderXLoader::ParseShaderSource(std::vector<std::string>& lines) {
	ReadShaderSource(lines);

	AssertX(type_ != ShaderTypeCount, "invalid shader file");
	answer_[type_] = globals_ + source_;

	return true;
}

bool ShaderXLoader::Preprocess(const std::string& line) {
	size_t pos = line.find(' ');
	AssertX(pos > 1, "unable to preprocess" + line);
	std::string cmd = line.substr(1, pos - 1);
	std::string parameter = Utility::Trim(line.substr(pos));

	if (cmd == SHADER) {
		return PreprocessShader(parameter);
	}
	else if (cmd == INCLUDE) {
		return PreprocessInclude(parameter);
	}

	source_ += line + '\n';
	return true;
}

ShaderType ShaderXLoader::ParseShaderType(const std::string& tag) {
	for (size_t i = 0; i < ShaderTypeCount; ++i) {
		if (tag == descriptions[i].tag) {
			return (ShaderType)i;
		}
	}

	AssertX(false, std::string("unkown shader tag ") + tag);
	return ShaderTypeCount;
}

bool ShaderXLoader::ReadShaderSource(std::vector<std::string> &lines) {
	for (size_t i = 0; i < lines.size(); ++i) {
		const std::string& line = lines[i];
		if (line.front() == '#' && !Preprocess(line)) {
			return false;
		}

		if (line.front() != '#') {
			source_ += line + '\n';
		}
	}

	return true;
}

bool ShaderXLoader::PreprocessShader(std::string parameter) {
	ShaderType newType = ParseShaderType(parameter);

	if (newType != type_) {
		if (type_ == ShaderTypeCount) {
			globals_ = source_;
		}
		else {
			if (!answer_[type_].empty()) {
				Debug::LogError(std::string(descriptions[type_].name) + " already exists");
				return false;
			}

			source_ = globals_ + source_;
			answer_[type_] = source_;
		}

		source_.clear();
		type_ = newType;
	}

	return true;
}

bool ShaderXLoader::PreprocessInclude(std::string &parameter) {
	std::vector<std::string> lines;
	if (!TextLoader::Load("shaders/" + parameter.substr(1, parameter.length() - 2), lines)) {
		return false;
	}

	return ReadShaderSource(lines);
}
