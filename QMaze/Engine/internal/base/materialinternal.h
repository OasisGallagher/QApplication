#pragma once

#include "material.h"
#include "internal/containers/ptrmap.h"
#include "internal/base/objectinternal.h"

class MaterialInternal : public IMaterial, public ObjectInternal {
	DEFINE_FACTORY_METHOD(Material)

public:
	MaterialInternal(Shader shader = Shader());
	~MaterialInternal() {}

public:
	virtual void SetDiffuseTexture(Texture value) { diffuse_ = value; }
	virtual Texture GetDiffuseTexture() { return diffuse_; }

	virtual void SetShader(Shader value);
	virtual Shader GetShader() { return shader_; }

	virtual void Bind();
	virtual void Unbind();

	virtual void SetInt(const std::string& name, int value);
	virtual void SetFloat(const std::string& name, float value);
	virtual void SetValue(const std::string& name, const void* value);
	virtual void SetTexture(const std::string& name, Texture texture);
	virtual void SetMatrix(const std::string& name, const glm::mat4& matrix);

	virtual void SetBlock(const std::string& name, const void* value);

private:
	struct Uniform {
		GLenum type;
		union { GLuint offset, location; };
		GLuint size, stride;
	};
	typedef PtrMap<Uniform> UniformContainer;

	struct UniformBlock {
		~UniformBlock();

		GLuint size, buffer, binding;
		UniformContainer uniforms;
	};
	typedef PtrMap<UniformBlock> UniformBlockContainer;

	void UpdateVariables();
	void AddAllUniforms();
	void AddAllUniformBlocks();
	void BindTextures();
	void UnbindTextures();
	GLuint GetSizeOfType(GLint type);
	bool IsSampler(const std::string& name);
	void SetUniform(struct Uniform* u, const void* value);
	GLuint GetUniformSize(GLint uniformType, GLint uniformSize, GLint uniformOffset, GLint uniformMatrixStride, GLint uniformArrayStride);

private:
	Shader shader_;
	Texture diffuse_;
	int maxTextureUnits_;
	std::vector<Texture> textures_;

	UniformContainer uniforms_;
	UniformBlockContainer blocks_;
};
