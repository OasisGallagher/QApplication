#include "shaderinternal.h"

#include "tools/debug.h"
#include "tools/string.h"
#include "internal/misc/loader.h"

ShaderDescription ShaderInternal::descriptions_[] =  {
	GL_VERTEX_SHADER, "VertexShader", "vert",
	GL_TESS_CONTROL_SHADER, "TessellationControlShader", "tesc",
	GL_TESS_EVALUATION_SHADER, "TessellationEvaluationShader", "tese",
	GL_GEOMETRY_SHADER, "GeometryShader", "geom",
	GL_FRAGMENT_SHADER, "FragmentShader", "frag"
};

ShaderInternal::ShaderInternal() : ObjectInternal(ObjectShader) {
	program_ = glCreateProgram();
	std::fill(shaderObjs_, shaderObjs_ + ShaderTypeCount, 0);
}

ShaderInternal::~ShaderInternal() {
	glDeleteProgram(program_);
	ClearIntermediateShaders();
}

bool ShaderInternal::Load(const std::string& path) {
	ShaderParser parser;
	std::string sources[ShaderTypeCount];
	if (!parser.Parse(path + ".glsl", sources)) {
		return false;
	}

	for (int i = 0; i < ShaderTypeCount; ++i) {
		if (!sources[i].empty() && !LoadSource((ShaderType)i, sources[i].c_str())) {
			return false;
		}
	}

	ClearIntermediateShaders();

	return true;
}

bool ShaderInternal::GetErrorMessage(GLuint shaderObj, std::string& answer) {
	if (shaderObj == 0) {
		answer = "invalid shader id";
		return false;
	}

	GLint length = 0, writen = 0;
	glGetShaderiv(shaderObj, GL_INFO_LOG_LENGTH, &length);
	if (length > 1) {
		char* log = new char[length];
		glGetShaderInfoLog(shaderObj, length, &writen, log);
		answer = log;
		delete[] log;
		return true;
	}

	return false;
}

bool ShaderInternal::Link() {
	glLinkProgram(program_);
	
	GLint status = GL_FALSE;
	glGetProgramiv(program_, GL_LINK_STATUS, &status);

	if (status != GL_TRUE) {
		Debug::LogError("failed to link shader.");
		return false;
	}

	glValidateProgram(program_);
	glGetProgramiv(program_, GL_VALIDATE_STATUS, &status);
	if (status != GL_TRUE) {
		Debug::LogError("failed to validate shader.");
		return false;
	}

	return true;
}

void ShaderInternal::ClearIntermediateShaders() {
	for (int i = 0; i < ShaderTypeCount; ++i) {
		if (shaderObjs_[i] != 0) {
			glDeleteShader(shaderObjs_[i]);
			shaderObjs_[i] = 0;
		}
	}
}

bool ShaderInternal::LoadSource(ShaderType shaderType, const char* source) {
	GLuint shaderObj = glCreateShader(Description(shaderType).glShaderType);

	glShaderSource(shaderObj, 1, &source, nullptr);
	glCompileShader(shaderObj);

	glAttachShader(program_, shaderObj);
	
	std::string message;
	if (!GetErrorMessage(shaderObj, message)) {
		if (shaderObjs_[shaderType] != 0) {
			glDeleteShader(shaderObjs_[shaderType]);
		}
		shaderObjs_[shaderType] = shaderObj;
		return true;
	}

	AssertX(false, Description(shaderType).name + std::string(" ") + message);
	return false;
}
