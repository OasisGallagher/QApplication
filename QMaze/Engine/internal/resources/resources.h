#pragma once
#include <map>

#include "shader.h"

class Resources {
public:
	static Shader FindShader(const std::string& path);

private:
	typedef std::map<std::string, Shader> ShaderContainer;
	static ShaderContainer shaders_;
};
