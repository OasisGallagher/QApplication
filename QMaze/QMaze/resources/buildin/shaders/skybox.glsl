#version 330

#shader vert

layout(location = 0) in vec3 c_position;

uniform mat4 c_modelToClipSpaceMatrix;

out vec3 UV;

void main() {
	vec4 pos = c_modelToClipSpaceMatrix * vec4(c_position, 1);
	gl_Position = pos.xyww;
	UV = c_position;
}

#shader frag

out vec4 color;

in vec3 UV;

uniform samplerCube c_diffuseTexture;

uniform bool test = false;

void main() {
	if (test) {
		color = vec4(1, 0, 0, 1);
	}
	else {
		color = texture(c_diffuseTexture, UV);
	}
}

