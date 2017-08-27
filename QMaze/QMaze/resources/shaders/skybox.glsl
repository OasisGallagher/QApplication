#version 330

#shader vert

layout(location = 0) in vec3 vert;

uniform mat4 MVP;

out vec3 UV;

 void main() {
	vec4 pos = MVP * vec4(vert, 1);
	gl_Position = pos.xyww;
	UV = vert;
 }

#shader frag

layout(location = 0) out vec4 color;

in vec3 UV;

uniform samplerCube textureSampler;

uniform bool test = false;

void main() {
	if (test) {
		color = vec4(1, 0, 0, 1);
	}
	else {
		color = texture(textureSampler, UV);
	}
}
