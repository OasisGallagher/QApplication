#pragma once
#define DEFINE_VARIABLE(name)	static const char* name = "c_" #name

namespace Variables {
	DEFINE_VARIABLE(position);
	DEFINE_VARIABLE(texCoord);
	DEFINE_VARIABLE(normal);
	DEFINE_VARIABLE(tangent);

	DEFINE_VARIABLE(fragColor);
	DEFINE_VARIABLE(depth);

	DEFINE_VARIABLE(mainTexture);
	DEFINE_VARIABLE(bumpTexture);
	DEFINE_VARIABLE(specularTexture);

	DEFINE_VARIABLE(gloss);

	DEFINE_VARIABLE(ambientLightColor);

	DEFINE_VARIABLE(lightColor);
	DEFINE_VARIABLE(lightPosition);
	DEFINE_VARIABLE(lightDirection);

	DEFINE_VARIABLE(cameraPosition);

	DEFINE_VARIABLE(localToClipSpaceMatrix);
	DEFINE_VARIABLE(localToWorldSpaceMatrix);
};

#undef DEFINE_VARIABLE
