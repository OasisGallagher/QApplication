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
	UniformMat4,
	UniformTexture,
};

class UniformVariable {
public:
	UniformVariable():type_(UniformNone) {}

public:
	int GetInt();
	bool GetBool();
	float GetFloat();
	glm::mat4 GetMat4();
	Texture GetTexture();
	int GetTextureIndex();

	UniformType GetType() const { return type_; }

	void SetInt(int value);
	void SetBool(bool value);
	void SetFloat(float value);
	void SetMat4(const glm::mat4& value);
	void SetTexture(Texture value);
	void SetTextureLocation(GLenum value);

private:
	union {
		int intValue_;
		bool boolValue_;
		float floatValue_;
		glm::mat4 mat4Value_;
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

struct UniformBlock {
	~UniformBlock();

	GLuint size, buffer, binding;
	UniformContainer uniforms;
};

typedef PtrMap<UniformBlock> UniformBlockContainer;
