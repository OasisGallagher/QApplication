#pragma once
#include <vector>
#include <string>
#include <memory>
#include <gl/glew.h>

#include "defs.h"
#include "object.h"
#include "ptrtable.h"

enum ShaderType {
	ShaderTypeVertex,
	ShaderTypeTessellationControl,
	ShaderTypeTessellationEvaluation,
	ShaderTypeGeometry,
	ShaderTypeFragment,
	ShaderTypeCount,
};

class ShaderPrivate : public Object {
public:
	ShaderPrivate();
	~ShaderPrivate();

public:
	bool Load(const std::string& path);
	bool Load(ShaderType shaderType, const std::string& path);
	bool Link();
	bool Bind();
	void Unbind();

public:
	GLuint GetProgram() const { return program_; }

	void SetUniform(const std::string& name, int value);
	void SetUniform(const std::string& name, float value);
	void SetUniform(const std::string& name, const void* value);

	void SetBlock(const std::string& name, const void* value);
	void SetBlockUniform(const std::string& blockName, const std::string& uniformName, const void* value);
	void SetBlockUniformArrayElement(const std::string& blockName, const std::string& uniformName, GLint index, const void* value);

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
};

class ShaderXLoader {
public:
	bool Load(const std::string& path, std::string* answer);

private:
	void Clear();
	bool ParseShaderSource(std::vector<std::string>& lines);
	bool ReadShaderSource(std::vector<std::string> &lines);

	bool Preprocess(const std::string& line);
	bool PreprocessInclude(std::string &parameter);
	bool PreprocessShader(std::string parameter);

	ShaderType ParseShaderType(const std::string& line);

private:
	ShaderType type_;
	std::string* answer_;
	std::string globals_;
	std::string source_;
};

class ENGINE_EXPORT Shader : public std::shared_ptr<ShaderPrivate> {
public:
	static Shader Find(const std::string& path);
};
