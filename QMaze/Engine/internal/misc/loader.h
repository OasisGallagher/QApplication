#pragma once
#include <vector>
#include <glm/glm.hpp>

#include "internal/base/shaderinternal.h"

class TextLoader {
public:
	static bool Load(const std::string& file, std::string& text);
	static bool Load(const std::string& file, std::vector<std::string>& lines);
};

class ShaderParser {
public:
	bool Parse(const std::string& path, std::string* answer);

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
