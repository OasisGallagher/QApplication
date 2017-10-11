#pragma once

#include "internal/base/shaderinternal.h"

class ShaderParser {
public:
	bool Parse(const std::string& path, std::string(&answer)[ShaderTypeCount]);

private:
	void Clear();
	bool ReadShaderSource(const std::vector<std::string> &lines);
	std::string FormatDefines(const std::string& defines);
	bool ParseShaderSource(const std::vector<std::string>& lines);

	bool Preprocess(const std::string& line);
	bool PreprocessInclude(const std::string& parameter);
	bool PreprocessShader(const std::string& parameter);

	void CalculateDefinesPermutations(std::vector<std::string>& anwser);

	ShaderType ParseShaderType(const std::string& line);

private:
	ShaderType type_;
	std::string path_;
	std::string* answer_;
	std::string globals_;
	std::string source_;
};
