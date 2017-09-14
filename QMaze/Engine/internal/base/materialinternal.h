#pragma once

#include "material.h"
#include "internal/base/uniform.h"
#include "internal/base/objectinternal.h"

class MaterialInternal : public IMaterial, public ObjectInternal {
	DEFINE_FACTORY_METHOD(Material)

public:
	MaterialInternal(Shader shader = Shader());
	~MaterialInternal() {}

public:
	virtual void SetShader(Shader value);
	virtual Shader GetShader() { return shader_; }

	virtual void Bind();
	virtual void Unbind();

	virtual void SetInt(const std::string& name, int value);
	virtual void SetFloat(const std::string& name, float value);
	virtual void SetTexture(const std::string& name, Texture texture);
	virtual void SetMatrix(const std::string& name, const glm::mat4& matrix);

	virtual void SetBlock(const std::string& name, const void* value);

private:
	bool IsSampler(int type);

	void UpdateVariables();
	void UpdateVertexAttributes();
	
	void AddAllUniforms();
	void AddAllUniformBlocks();
	
	void BindTextures();
	void UnbindTextures();
	
	GLuint GetSizeOfType(GLint type);
	GLuint GetUniformSize(GLint uniformType, GLint uniformSize, GLint uniformOffset, GLint uniformMatrixStride, GLint uniformArrayStride);
	
	void SetUniform(struct Uniform* u, const void* value);
	void SetValue(const std::string& name, const void* value);

private:
	Shader shader_;
	Texture diffuse_;
	int oldProgram_;
	int maxTextureUnits_;
	int textureUnitIndex_;
	UniformContainer uniforms_;
	UniformBlockContainer blocks_;
};
