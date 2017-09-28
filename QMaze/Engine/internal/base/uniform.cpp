#include "uniform.h"
#include "tools/debug.h"

std::string UniformVariable::UniformTypeToName(UniformType type) {
	static const char* uniformTypeName[] {
		"null",
		"int",
		"bool",
		"float",
		"mat4",
		"vec3",
		"sampler",
	};

	return uniformTypeName[type];
}

int UniformVariable::GetInt() {
	AssertX(type_ == UniformInt, "invalid uniform type.");
	return intValue_;
}

bool UniformVariable::GetBool() {
	AssertX(type_ == UniformBool, "invalid uniform type.");
	return boolValue_;
}

float UniformVariable::GetFloat() {
	AssertX(type_ == UniformFloat, "invalid uniform type.");
	return floatValue_;
}

glm::mat4 UniformVariable::GetMatrix4() {
	AssertX(type_ == UniformMatrix4, "invalid uniform type.");
	return mat4Value_;
}

glm::vec3 UniformVariable::GetVector3() {
	AssertX(type_ == UniformVector3, "invalid uniform type.");
	return vector3Value_;
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

void UniformVariable::SetBool(bool value) {
	type_ = UniformBool;
	boolValue_ = value;
}

void UniformVariable::SetFloat(float value) {
	type_ = UniformFloat;
	floatValue_ = value;
}

void UniformVariable::SetMatrix4(const glm::mat4& value) {
	type_ = UniformMatrix4;
	mat4Value_ = value;
}

void UniformVariable::SetVector3(const glm::vec3& value) {
	type_ = UniformVector3;
	vector3Value_ = value;
}

void UniformVariable::SetTexture(Texture value) {
	type_ = UniformTexture;
	texture_ = value;
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
