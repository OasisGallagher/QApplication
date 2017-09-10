#pragma once
#include <vector>
#include <gl/glew.h>

#include "shader.h"
#include "internal/misc/ptrtable.h"
#include "internal/base/objectinternal.h"

enum ShaderType {
	ShaderTypeVertex,
	ShaderTypeTessellationControl,
	ShaderTypeTessellationEvaluation,
	ShaderTypeGeometry,
	ShaderTypeFragment,
	ShaderTypeCount,
};

struct ShaderDescription {
	GLenum glShaderType;
	const char* name;
	const char* tag;
};

class ShaderInternal : public Shader, public ObjectInternal {
public:
	static const ShaderDescription& Description(ShaderType shaderType) {
		return descriptions_[shaderType];
	}

public:
	ShaderInternal();
	~ShaderInternal();

public:
	virtual bool Load(const std::string& path);
	virtual bool Load(ShaderType shaderType, const std::string& path);
	virtual bool Link();
	virtual bool Bind();
	virtual void Unbind();

public:
	virtual GLuint GetProgram() const { return program_; }

	virtual void SetUniform(const std::string& name, int value);
	virtual void SetUniform(const std::string& name, float value);
	virtual void SetUniform(const std::string& name, const void* value);

	virtual void SetBlock(const std::string& name, const void* value);
	virtual void SetBlockUniform(const std::string& blockName, const std::string& uniformName, const void* value);
	virtual void SetBlockUniformArrayElement(const std::string& blockName, const std::string& uniformName, GLint index, const void* value);

private:
	bool LinkShader();
	void ClearShaders();
	bool LoadShader(ShaderType shaderType, const char* source);

	void AddAllBlocks();
	void AddAllUniforms();

	GLuint GetSizeOfType(GLint type);
	GLuint GetUniformSize(GLint uniformType, GLint uniformSize, GLint uniformOffset, GLint uniformMatrixStride, GLint uniformArrayStride);

	bool GetErrorMessage(GLuint shaderObj, std::string& answer);
	GLuint GenerateBindingIndex() const;

private:
	struct Uniform {
		GLenum type;
		union {
			GLuint offset;
			GLuint location;
		};

		GLuint size;
		GLuint stride;
	};

	typedef PtrTable<Uniform> Uniforms;

	struct UniformBlock {
		UniformBlock();
		~UniformBlock();

		GLuint size;
		GLuint buffer;
		GLuint binding;
		
		Uniforms uniforms;
	};

	typedef PtrTable<UniformBlock> UniformBlocks;

private:
	Uniforms uniforms_;
	UniformBlocks blocks_;

	GLuint program_;
	GLuint shaderObjs_[ShaderTypeCount];

	static ShaderDescription descriptions_[ShaderTypeCount];
};