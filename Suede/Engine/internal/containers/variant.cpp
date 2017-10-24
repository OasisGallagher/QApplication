#include "variant.h"
#include "tools/debug.h"

std::string Variant::TypeString(VariantType type) {
	static const char* variantTypeNames[]{
		"null",
		"int",
		"bool",
		"float",
		"mat4",
		"vec3",
		"sampler",
		"quaternion",
	};

	return variantTypeNames[type];
}

int Variant::GetInt() {
	AssertX(type_ == VariantInt, "invalid variant type.");
	return intValue_;
}

bool Variant::GetBool() {
	AssertX(type_ == VariantBool, "invalid uniform type.");
	return boolValue_;
}

float Variant::GetFloat() {
	AssertX(type_ == VariantFloat, "invalid uniform type.");
	return floatValue_;
}

glm::mat4 Variant::GetMatrix4() {
	AssertX(type_ == VariantMatrix4, "invalid uniform type.");
	return mat4Value_;
}

glm::vec3 Variant::GetVector3() {
	AssertX(type_ == VariantVector3, "invalid uniform type.");
	return vector3Value_;
}

glm::quat Variant::GetQuaternion() {
	AssertX(type_ == VariantQuaternion, "invalid uniform type.");
	return quaternionValue_;
}

Texture Variant::GetTexture() {
	AssertX(type_ == VariantTexture, "invalid uniform type.");
	return texture_;
}

int Variant::GetTextureIndex() {
	AssertX(type_ == VariantTexture, "invalid uniform type.");
	return textureIndex_;
}

void Variant::SetInt(int value) {
	type_ = VariantInt;
	intValue_ = value;
}

void Variant::SetBool(bool value) {
	type_ = VariantBool;
	boolValue_ = value;
}

void Variant::SetFloat(float value) {
	type_ = VariantFloat;
	floatValue_ = value;
}

void Variant::SetMatrix4(const glm::mat4& value) {
	type_ = VariantMatrix4;
	mat4Value_ = value;
}

void Variant::SetVector3(const glm::vec3& value) {
	type_ = VariantVector3;
	vector3Value_ = value;
}

void Variant::SetQuaternion(const glm::quat& value) {
	type_ = VariantQuaternion;
	quaternionValue_ = value;
}

void Variant::SetTexture(Texture value) {
	type_ = VariantTexture;
	texture_ = value;
}

void Variant::SetTextureLocation(GLenum value) {
	type_ = VariantTexture;
	textureIndex_ = value;
}
