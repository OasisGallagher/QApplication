#pragma once
#include <glm/glm.hpp>

#include "object.h"
#include "shader.h"
#include "texture.h"

class ENGINE_EXPORT IMaterial : virtual public IObject {
public:
	virtual void SetShader(Shader shader) = 0;
	virtual Shader GetShader() = 0;

	virtual void SetInt(const std::string& name, int value) = 0;
	virtual void SetFloat(const std::string& name, float value) = 0;
	virtual void SetValue(const std::string& name, const void* value) = 0;
	virtual void SetTexture(const std::string& name, Texture texture) = 0;
	virtual void SetMatrix(const std::string& name, const glm::mat4& matrix) = 0;

	virtual void SetBlock(const std::string& name, const void* value) = 0;

	/*virtual void SetBlockUniform(const std::string& blockName,
		const std::string& uniformName, const void* value) = 0;

	virtual void SetBlockUniformArrayElement(const std::string& blockName,
		const std::string& uniformName, GLint index, const void* value) = 0;
	*/
};

typedef smart_ptr<IMaterial> Material;
