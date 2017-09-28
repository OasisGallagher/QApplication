#include <fstream>

#include "loader.h"
#include "tools/debug.h"
#include "tools/string.h"

static const char* SHADER = "shader";
static const char* INCLUDE = "include";
static const char* GLSL_VERSION = "#version 330\n";

bool TextLoader::Load(const std::string& file, std::string& text) {
	std::ifstream ifs(file, std::ios::in);
	if (!ifs) {
		Debug::LogError("failed to open file " + file + ".");
		return false;
	}

	text.clear();
	const char* seperator = "";

	std::string line;
	for (; getline(ifs, line);) {
		if (!line.empty()) {
			text += seperator;
			text += line;
			seperator = "\n";
		}
	}

	ifs.close();

	return true;
}

bool TextLoader::Load(const std::string& file, std::vector<std::string>& lines) {
	std::ifstream ifs(file, std::ios::in);
	if (!ifs) {
		Debug::LogError("failed to open file " + file + ".");
		return false;
	}

	lines.clear();

	std::string line;
	for (; getline(ifs, line);) {
		if (!line.empty()) {
			lines.push_back(line);
		}
	}

	ifs.close();

	return true;
}

bool ShaderParser::Parse(const std::string& path, std::string* answer) {
	std::vector<std::string> lines;
	if (!TextLoader::Load("resources/" + path, lines)) {
		return false;
	}

	Clear();

	answer_ = answer;
	return ParseShaderSource(lines);
}

void ShaderParser::Clear() {
	type_ = ShaderTypeCount;
	source_.clear();
	globals_.clear();
	answer_ = nullptr;
}

bool ShaderParser::ParseShaderSource(std::vector<std::string>& lines) {
	globals_ = GLSL_VERSION;
	ReadShaderSource(lines);

	AssertX(type_ != ShaderTypeCount, "invalid shader file");
	answer_[type_] = globals_ + source_;

	return true;
}

bool ShaderParser::Preprocess(const std::string& line) {
	size_t pos = line.find(' ');
	AssertX(pos > 1, "unable to preprocess" + line);
	std::string cmd = line.substr(1, pos - 1);
	std::string parameter = String::Trim(line.substr(pos));

	if (cmd == SHADER) {
		return PreprocessShader(parameter);
	}
	else if (cmd == INCLUDE) {
		return PreprocessInclude(parameter);
	}

	source_ += line + '\n';
	return true;
}

ShaderType ShaderParser::ParseShaderType(const std::string& tag) {
	for (size_t i = 0; i < ShaderTypeCount; ++i) {
		if (tag == ShaderInternal::Description((ShaderType)i).tag) {
			return (ShaderType)i;
		}
	}

	AssertX(false, std::string("unkown shader tag ") + tag);
	return ShaderTypeCount;
}

bool ShaderParser::ReadShaderSource(std::vector<std::string> &lines) {
	for (size_t i = 0; i < lines.size(); ++i) {
		const std::string& line = lines[i];
		if (line.front() == '#' && !Preprocess(line)) {
			return false;
		}

		if (line.front() != '#') {
			source_ += line + '\n';
		}
	}

	return true;
}

bool ShaderParser::PreprocessShader(std::string parameter) {
	ShaderType newType = ParseShaderType(parameter);

	if (newType != type_) {
		if (type_ == ShaderTypeCount) {
			globals_ += source_;
		}
		else {
			if (!answer_[type_].empty()) {
				Debug::LogError(std::string(ShaderInternal::Description(type_).name) + " already exists");
				return false;
			}

			source_ = globals_ + source_;
			answer_[type_] = source_;
		}

		source_.clear();
		type_ = newType;
	}

	return true;
}

bool ShaderParser::PreprocessInclude(std::string &parameter) {
	std::vector<std::string> lines;
	if (!TextLoader::Load("shaders/" + parameter.substr(1, parameter.length() - 2), lines)) {
		return false;
	}

	return ReadShaderSource(lines);
}
