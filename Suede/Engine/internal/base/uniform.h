#pragma once
#include <gl/glew.h>
#include <glm/glm.hpp>

#include "texture.h"
#include "internal/containers/ptrmap.h"

enum UniformType {
	UniformNone,
	UniformInt,
	UniformBool,
	UniformFloat,
	UniformMatrix4,
	UniformVector3,
	UniformTexture,
};

class UniformVariable {
public:
	UniformVariable():type_(UniformNone) {}

public:
	static std::string UniformTypeToName(UniformType type);

public:
	int GetInt();
	bool GetBool();
	float GetFloat();
	glm::mat4 GetMatrix4();
	glm::vec3 GetVector3();
	Texture GetTexture();
	int GetTextureIndex();

	UniformType GetType() { return type_; }

	void SetInt(int value);
	void SetBool(bool value);
	void SetFloat(float value);
	void SetMatrix4(const glm::mat4& value);
	void SetVector3(const glm::vec3& value);
	void SetTexture(Texture value);
	void SetTextureLocation(GLenum value);

private:
	union {
		int intValue_;
		bool boolValue_;
		float floatValue_;
		glm::mat4 mat4Value_;
		glm::vec3 vector3Value_;
		int textureIndex_;
	};

	Texture texture_;

	UniformType type_;
};

struct Uniform {
	GLenum type;
	union { GLuint offset, location; };
	GLuint size, stride;
	UniformVariable value;
};

typedef PtrMap<Uniform> UniformContainer;