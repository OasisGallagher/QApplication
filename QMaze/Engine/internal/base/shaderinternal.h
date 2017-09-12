#pragma once
#include <vector>
#include <gl/glew.h>

#include "shader.h"
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

class ShaderInternal : public IShader, public ObjectInternal {
	DEFINE_FACTORY_METHOD(Shader)

public:
	static const ShaderDescription& Description(ShaderType shaderType) {
		return descriptions_[shaderType];
	}

public:
	ShaderInternal();
	~ShaderInternal();

public:
	virtual bool Load(const std::string& path);
	virtual GLuint GetNativePointer() const { return program_; }

public:
	virtual void SetUniform(const std::string& name, int value);
	virtual void SetUniform(const std::string& name, float value);
	virtual void SetUniform(const std::string& name, const void* value);

	virtual void SetBlock(const std::string& name, const void* value);
	virtual void SetBlockUniform(const std::string& blockName, const std::string& uniformName, const void* value);
	virtual void SetBlockUniformArrayElement(const std::string& blockName, const std::string& uniformName, GLint index, const void* value);

private:
	bool Link();
	bool LoadSource(ShaderType shaderType, const char* source);
	void ClearIntermediateShaders();

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

	typedef PtrMap<Uniform> Uniforms;

	struct UniformBlock {
		UniformBlock();
		~UniformBlock();

		GLuint size;
		GLuint buffer;
		GLuint binding;
		
		Uniforms uniforms;
	};

	typedef PtrMap<UniformBlock> UniformBlocks;

private:
	Uniforms uniforms_;
	UniformBlocks blocks_;

	GLuint program_;
	GLuint shaderObjs_[ShaderTypeCount];

	static ShaderDescription descriptions_[ShaderTypeCount];
};