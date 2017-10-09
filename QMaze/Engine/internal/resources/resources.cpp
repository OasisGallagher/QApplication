#include "resources.h"
#include "internal/memory/factory.h"
#include "internal/base/glsldefines.h"
#include "internal/base/shaderinternal.h"

Resources::ShaderContainer Resources::shaders_;

Shader Resources::FindShader(const std::string& path) {
	ShaderContainer::iterator ite = shaders_.find(path);
	if (ite != shaders_.end()) {
		return ite->second;
	}

	Shader shader = Factory::Create<ShaderInternal>();
	shaders_.insert(std::make_pair(path, shader));
	if (shader->Load(path + GLSL_POSTFIX)) {
		return shader;
	}

	return Shader();
}
