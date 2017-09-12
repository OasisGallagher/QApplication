#include "materialinternal.h"

MaterialInternal::MaterialInternal(Shader shader)
	: ObjectInternal(ObjectTypeMaterial), shader_(shader)
	, maxTextureUnits_(0) {
	glGetIntegerv(GL_MAX_TEXTURE_UNITS, &maxTextureUnits_);
}

void MaterialInternal::SetShader(Shader value) {
	shader_ = value;
	
	UnbindTextures();
	textures_.clear();

	UpdateVariables();
}

void MaterialInternal::SetInt(const std::string& name, int value) {
	AssertX(uniforms_.contains(name), "invalid uniform " + name + ".");

	Uniform* u = uniforms_[name];
	glProgramUniform1i(shader_->GetNativePointer(), u->location, value);
}

void MaterialInternal::SetFloat(const std::string& name, float value) {
	AssertX(uniforms_.contains(name), "invalid uniform " + name + ".");

	Uniform* u = uniforms_[name];
	glProgramUniform1f(shader_->GetNativePointer(), u->location, value);
}

void MaterialInternal::SetValue(const std::string& name, const void* value) {
	AssertX(uniforms_.contains(name), "invalid uniform " + name + ".");

	Uniform* u = uniforms_[name];
	SetUniform(u, value);
}

void MaterialInternal::SetTexture(const std::string& name, Texture texture) {
	AssertX(uniforms_.contains(name), "invalid uniform " + name + ".");
	AssertX(IsSampler(name), "invalid sampler " + name + ".");
	AssertX(textures_.size() < maxTextureUnits_, "too many textures.");

	Uniform* u = uniforms_[name];
	textures_.push_back(texture);
	glProgramUniform1f(shader_->GetNativePointer(), u->location, textures_.size() - 1);
}

void MaterialInternal::SetMatrix(const std::string& name, const glm::mat4& matrix) {
	SetValue(name, &matrix);
}

void MaterialInternal::SetBlock(const std::string& name, const void* value) {
	Assert(blocks_.contains(name), "invalid block name " + name + ".");

	UniformBlock* block = blocks_[name];
	glBindBuffer(GL_UNIFORM_BUFFER, block->buffer);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, block->size, value);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void MaterialInternal::Bind() {
	BindTextures();
}

void MaterialInternal::Unbind() {
	UnbindTextures();
}

void MaterialInternal::UpdateVariables() {
	// http://www.lighthouse3d.com/tutorials/glsl-tutorial/uniform-blocks/
	AddAllUniforms();
	AddAllUniformBlocks();
}

void MaterialInternal::BindTextures() {
	for (int i = 0; i < textures_.size(); ++i) {
		textures_[i]->Bind(GL_TEXTURE0 + i);
	}
}

void MaterialInternal::UnbindTextures() {
	for (int i = 0; i < textures_.size(); ++i) {
		textures_[i]->Unbind();
	}
}

bool MaterialInternal::IsSampler(const std::string& name) {
	Uniform* u = uniforms_[name];
	if (u == nullptr) { return false; }
	switch (u->type) {
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
			return true;
	}

	return false;
}

void MaterialInternal::AddAllUniforms() {
	uniforms_.clear();

	GLenum type;
	GLuint location = 0;
	GLint size, count, maxLength, length, stride;

	GLuint program = shader_->GetNativePointer();
	glGetProgramiv(program, GL_ACTIVE_UNIFORMS, &count);
	glGetProgramiv(program, GL_ACTIVE_UNIFORM_MAX_LENGTH, &maxLength);

	char* name = new char[maxLength];
	for (int i = 0; i < count; ++i) {
		glGetActiveUniform(program, i, maxLength, &length, &size, &type, name);

		location = glGetUniformLocation(program, name);

		// -1 indicates that is not an active uniform, although it may be present in a
		// uniform block.
		if (location == GL_INVALID_INDEX) {
			continue;
		}

		stride = -1;

		// 当shader中有uniform array时, GL_INVALID_OPERATION.
		//glGetActiveUniformsiv(program, 1, &location, GL_UNIFORM_ARRAY_STRIDE, &stride);

		Uniform* uniform = uniforms_[name];
		uniform->type = type;
		uniform->location = location;
		uniform->size = size;
		uniform->stride = stride;
	}

	delete[] name;
}

void MaterialInternal::AddAllUniformBlocks() {
	blocks_.clear();

	GLsizei nameWritten;
	GLint blockCount, uniformCount, blockNameLength, uniformNameLength, dataSize;
	GLint uniformType, uniformSize, uniformOffset, uniformMatrixStride, uniformArrayStride;

	GLuint buffer;
	std::string blockName, uniformName;

	GLuint program = shader_->GetNativePointer();

	glGetProgramiv(program, GL_ACTIVE_UNIFORM_BLOCKS, &blockCount);
	for (GLint i = 0; i < blockCount; ++i) {
		glGetActiveUniformBlockiv(program, i, GL_UNIFORM_BLOCK_NAME_LENGTH, &blockNameLength);

		blockName.resize(blockNameLength);
		glGetActiveUniformBlockName(program, i, blockNameLength, &nameWritten, &blockName[0]);
		blockName.resize(nameWritten);

		if (blocks_.contains(blockName)) {
			glUniformBlockBinding(program, i, blocks_[blockName]->binding);
			continue;
		}

		UniformBlock* block = blocks_[blockName];

		glGetActiveUniformBlockiv(program, i, GL_UNIFORM_BLOCK_DATA_SIZE, &dataSize);

		glGenBuffers(1, &buffer);
		glBindBuffer(GL_UNIFORM_BUFFER, buffer);
		glBufferData(GL_UNIFORM_BUFFER, dataSize, NULL, GL_DYNAMIC_DRAW);

		static GLuint bindingIndex = 0;
		++bindingIndex;

		glUniformBlockBinding(program, i, bindingIndex);
		glBindBufferRange(GL_UNIFORM_BUFFER, bindingIndex, buffer, 0, dataSize);

		glGetActiveUniformBlockiv(program, i, GL_UNIFORM_BLOCK_ACTIVE_UNIFORMS, &uniformCount);

		GLuint* indices = new GLuint[uniformCount];
		glGetActiveUniformBlockiv(program, i, GL_UNIFORM_BLOCK_ACTIVE_UNIFORM_INDICES, (GLint*)indices);

		glGetProgramiv(program, GL_ACTIVE_UNIFORM_MAX_LENGTH, &uniformNameLength);
		uniformName.resize(uniformNameLength);

		for (GLint j = 0; j < uniformCount; ++j) {
			glGetActiveUniformName(program, indices[j], uniformNameLength, &nameWritten, &uniformName[0]);
			uniformName.resize(nameWritten);

			Uniform* uniform = block->uniforms[uniformName];

			// for uniform Matrices { mat4 m };
			// uniformSize = 1, uniformMatrixStride = 16, uniformArrayStride = 0.
			// for uniform Matrices { mat4 m[2] }:
			// uniformSize = 2, uniformMatrixStride = 16, uniformArrayStride = 64.
			// for uniform Matrices { float m[4] }:
			// uniformSize = 4, uniformMatrixStride = 0, uniformArrayStride = 4.
			glGetActiveUniformsiv(program, 1, &indices[j], GL_UNIFORM_TYPE, &uniformType);
			glGetActiveUniformsiv(program, 1, &indices[j], GL_UNIFORM_SIZE, &uniformSize);
			glGetActiveUniformsiv(program, 1, &indices[j], GL_UNIFORM_OFFSET, &uniformOffset);
			glGetActiveUniformsiv(program, 1, &indices[j], GL_UNIFORM_MATRIX_STRIDE, &uniformMatrixStride);
			glGetActiveUniformsiv(program, 1, &indices[j], GL_UNIFORM_ARRAY_STRIDE, &uniformArrayStride);

			uniform->offset = uniformOffset;
			uniform->type = uniformType;
			uniform->size = GetUniformSize(uniformType, uniformSize, uniformOffset, uniformMatrixStride, uniformArrayStride);
			uniform->stride = uniformArrayStride;
		}

		block->size = dataSize;
		block->binding = bindingIndex;
		block->buffer = buffer;
	}
}

GLuint MaterialInternal::GetUniformSize(GLint uniformType, GLint uniformSize,
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

GLuint MaterialInternal::GetSizeOfType(GLint type) {
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

void MaterialInternal::SetUniform(Uniform* u, const void* value) {
	GLuint program = shader_->GetNativePointer();

	switch (u->type) {
		// Floats
		case GL_FLOAT:
			glProgramUniform1fv(program, u->location, u->size, (const GLfloat *)value);
			break;
		case GL_FLOAT_VEC2:
			glProgramUniform2fv(program, u->location, u->size, (const GLfloat *)value);
			break;
		case GL_FLOAT_VEC3:
			glProgramUniform3fv(program, u->location, u->size, (const GLfloat *)value);
			break;
		case GL_FLOAT_VEC4:
			glProgramUniform4fv(program, u->location, u->size, (const GLfloat *)value);
			break;

			// Doubles
		case GL_DOUBLE:
			glProgramUniform1dv(program, u->location, u->size, (const GLdouble *)value);
			break;
		case GL_DOUBLE_VEC2:
			glProgramUniform2dv(program, u->location, u->size, (const GLdouble *)value);
			break;
		case GL_DOUBLE_VEC3:
			glProgramUniform3dv(program, u->location, u->size, (const GLdouble *)value);
			break;
		case GL_DOUBLE_VEC4:
			glProgramUniform4dv(program, u->location, u->size, (const GLdouble *)value);
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
			glProgramUniform1iv(program, u->location, u->size, (const GLint *)value);
			break;
		case GL_BOOL_VEC2:
		case GL_INT_VEC2:
			glProgramUniform2iv(program, u->location, u->size, (const GLint *)value);
			break;
		case GL_BOOL_VEC3:
		case GL_INT_VEC3:
			glProgramUniform3iv(program, u->location, u->size, (const GLint *)value);
			break;
		case GL_BOOL_VEC4:
		case GL_INT_VEC4:
			glProgramUniform4iv(program, u->location, u->size, (const GLint *)value);
			break;

			// Unsigned ints
		case GL_UNSIGNED_INT:
			glProgramUniform1uiv(program, u->location, u->size, (const GLuint *)value);
			break;
		case GL_UNSIGNED_INT_VEC2:
			glProgramUniform2uiv(program, u->location, u->size, (const GLuint *)value);
			break;
		case GL_UNSIGNED_INT_VEC3:
			glProgramUniform3uiv(program, u->location, u->size, (const GLuint *)value);
			break;
		case GL_UNSIGNED_INT_VEC4:
			glProgramUniform4uiv(program, u->location, u->size, (const GLuint *)value);
			break;

			// Float Matrices
		case GL_FLOAT_MAT2:
			glProgramUniformMatrix2fv(program, u->location, u->size, false, (const GLfloat *)value);
			break;
		case GL_FLOAT_MAT3:
			glProgramUniformMatrix3fv(program, u->location, u->size, false, (const GLfloat *)value);
			break;
		case GL_FLOAT_MAT4:
			glProgramUniformMatrix4fv(program, u->location, u->size, false, (const GLfloat *)value);
			break;
		case GL_FLOAT_MAT2x3:
			glProgramUniformMatrix2x3fv(program, u->location, u->size, false, (const GLfloat *)value);
			break;
		case GL_FLOAT_MAT2x4:
			glProgramUniformMatrix2x4fv(program, u->location, u->size, false, (const GLfloat *)value);
			break;
		case GL_FLOAT_MAT3x2:
			glProgramUniformMatrix3x2fv(program, u->location, u->size, false, (const GLfloat *)value);
			break;
		case GL_FLOAT_MAT3x4:
			glProgramUniformMatrix3x4fv(program, u->location, u->size, false, (const GLfloat *)value);
			break;
		case GL_FLOAT_MAT4x2:
			glProgramUniformMatrix4x2fv(program, u->location, u->size, false, (const GLfloat *)value);
			break;
		case GL_FLOAT_MAT4x3:
			glProgramUniformMatrix4x3fv(program, u->location, u->size, false, (const GLfloat *)value);
			break;

			// Double Matrices
		case GL_DOUBLE_MAT2:
			glProgramUniformMatrix2dv(program, u->location, u->size, false, (const GLdouble *)value);
			break;
		case GL_DOUBLE_MAT3:
			glProgramUniformMatrix3dv(program, u->location, u->size, false, (const GLdouble *)value);
			break;
		case GL_DOUBLE_MAT4:
			glProgramUniformMatrix4dv(program, u->location, u->size, false, (const GLdouble *)value);
			break;
		case GL_DOUBLE_MAT2x3:
			glProgramUniformMatrix2x3dv(program, u->location, u->size, false, (const GLdouble *)value);
			break;
		case GL_DOUBLE_MAT2x4:
			glProgramUniformMatrix2x4dv(program, u->location, u->size, false, (const GLdouble *)value);
			break;
		case GL_DOUBLE_MAT3x2:
			glProgramUniformMatrix3x2dv(program, u->location, u->size, false, (const GLdouble *)value);
			break;
		case GL_DOUBLE_MAT3x4:
			glProgramUniformMatrix3x4dv(program, u->location, u->size, false, (const GLdouble *)value);
			break;
		case GL_DOUBLE_MAT4x2:
			glProgramUniformMatrix4x2dv(program, u->location, u->size, false, (const GLdouble *)value);
			break;
		case GL_DOUBLE_MAT4x3:
			glProgramUniformMatrix4x3dv(program, u->location, u->size, false, (const GLdouble *)value);
			break;
	}
}

MaterialInternal::UniformBlock::~UniformBlock() {
	if (buffer != 0) {
		glDeleteBuffers(1, &buffer);
	}
}
