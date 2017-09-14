#include "uniform.h"
#include "tools/debug.h"

int UniformVariable::GetInt() {
	AssertX(type_ == UniformInt, "invalid uniform type.");
	return intValue_;
}

float UniformVariable::GetFloat() {
	AssertX(type_ == UniformFloat, "invalid uniform type.");
	return floatValue_;
}

glm::mat4 UniformVariable::GetMat4() {
	AssertX(type_ == UniformMat4, "invalid uniform type.");
	return mat4Value_;
}

Texture UniformVariable::GetTexture() {
	AssertX(type_ == UniformTexture, "invalid uniform type.");
	return texture_;
}

int UniformVariable::GetTextureIndex() {
	AssertX(type_ == UniformTexture, "invalid uniform type.");
	return textureIndex_;
}

void UniformVariable::SetInt(int value) {
	type_ = UniformInt;
	intValue_ = value;
}

void UniformVariable::SetFloat(float value) {
	type_ = UniformFloat;
	floatValue_ = value;
}

void UniformVariable::SetMat4(const glm::mat4& value) {
	type_ = UniformMat4;
	mat4Value_ = value;
}

void UniformVariable::SetTexture(Texture value) {
	type_ = UniformTexture;
	texture_ = texture;
}

void UniformVariable::SetTextureLocation(GLenum value) {
	type_ = UniformTexture;
	textureIndex_ = value;
}

UniformBlock::~UniformBlock() {
	if (buffer != 0) {
		glDeleteBuffers(1, &buffer);
	}
}
