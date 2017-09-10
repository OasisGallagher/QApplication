#pragma once
#include <vector>
#include <string>
#include <memory>
#include <gl/glew.h>

#include "object.h"

class ENGINE_EXPORT Shader : virtual public Object {
public:
	virtual bool Load(const std::string& path) = 0;
	virtual bool Link() = 0;
	virtual bool Bind() = 0;
	virtual void Unbind() = 0;

	virtual GLuint GetProgram() const = 0;

	virtual void SetUniform(const std::string& name, int value) = 0;
	virtual void SetUniform(const std::string& name, float value) = 0;
	virtual void SetUniform(const std::string& name, const void* value) = 0;

	virtual void SetBlock(const std::string& name, const void* value) = 0;
	
	virtual void SetBlockUniform(const std::string& blockName,
		const std::string& uniformName, const void* value) = 0;

	virtual void SetBlockUniformArrayElement(const std::string& blockName,
		const std::string& uniformName, GLint index, const void* value) = 0;
};
