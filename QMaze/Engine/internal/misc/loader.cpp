#include <fstream>

#include "loader.h"
#include "tools/debug.h"
#include "tools/string.h"
#include "internal/base/glsldefines.h"

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

bool ShaderParser::Parse(const std::string& path, std::string(&answer)[ShaderTypeCount]) {
	std::vector<std::string> lines;
	if (!TextLoader::Load("resources/" + path, lines)) {
		return false;
	}

	Clear();

	answer_ = answer;
	path_ = path;
	return ParseShaderSource(lines);
}

void ShaderParser::Clear() {
	type_ = ShaderTypeCount;
	source_.clear();
	globals_.clear();
	answer_ = nullptr;
}

bool ShaderParser::ParseShaderSource(const std::vector<std::string>& lines) {
	globals_ = "#version " GLSL_VERSION "\n";
	//globals_ += FormatDefines(defines);
	ReadShaderSource(lines);

	AssertX(type_ != ShaderTypeCount, "invalid shader file");
	answer_[type_] = globals_ + source_;

	return true;
}

std::string ShaderParser::FormatDefines(const std::string& defines) {
	std::vector<std::string> container;
	String::Split(defines, '|', container);

	std::string ans;
	for (int i = 0; i < container.size(); ++i) {
		ans += "#define " + container[i] + "\n";
	}

	return ans;
}

bool ShaderParser::Preprocess(const std::string& line) {
	size_t pos = line.find(' ');
	std::string cmd = line.substr(1, pos - 1);
	std::string parameter;
	if (pos != std::string::npos) {
		parameter = String::Trim(line.substr(pos));
	}

	if (cmd == GLSL_TAG_SHADER) {
		return PreprocessShader(parameter);
	}

	if (cmd == GLSL_TAG_INCLUDE) {
		std::string old = path_;
		bool status = PreprocessInclude(parameter);
		path_ = old;
		return status;
	}

	source_ += line + '\n';
	return true;
}

void ShaderParser::CalculateDefinesPermutations(std::vector<std::string>& anwser) {
	std::vector<std::string> defines_;
	int max = 1 << 0; defines_.size();
	for (int i = 0; i < max; ++i) {
		std::string perm;
		const char* sep = "";
		for (int j = 0; j < defines_.size(); ++j) {
			if (((1 << j) & i) != 0) {
				perm += sep;
				sep = "|";
				perm += defines_[j];
			}
		}

		anwser.push_back(perm);
	}
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

bool ShaderParser::ReadShaderSource(const std::vector<std::string> &lines) {
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

bool ShaderParser::PreprocessShader(const std::string& parameter) {
	ShaderType newType = ParseShaderType(parameter);

	if (newType != type_) {
		if (type_ == ShaderTypeCount) {
			globals_ += source_;
		}
		else {
			if (!answer_[type_].empty()) {
				Debug::LogError(std::string(ShaderInternal::Description(type_).name) + " already exists.");
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

bool ShaderParser::PreprocessInclude(const std::string& parameter) {
	std::vector<std::string> lines;
	std::string path = parameter.substr(1, parameter.length() - 2);
	if (!TextLoader::Load("resources/" + path, lines)) {
		return false;
	}

	path_ = path;

	return ReadShaderSource(lines);
}
