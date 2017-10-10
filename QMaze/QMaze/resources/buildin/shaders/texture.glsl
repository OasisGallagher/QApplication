#shader vertex
in vec3 c_position;
in vec2 c_texCoord;
in vec3 c_normal;

out vec2 texCoord;
out vec3 worldPos;
out vec3 normal;

uniform mat4 c_localToClipSpaceMatrix;
uniform mat4 c_localToWorldSpaceMatrix;

void main() {
	texCoord = c_texCoord;

	normal = (c_localToWorldSpaceMatrix * vec4(c_normal, 0)).xyz;
	worldPos = (c_localToWorldSpaceMatrix * vec4(c_position, 1)).xyz;

	gl_Position = c_localToClipSpaceMatrix * vec4(c_position, 1);
}

#shader fragment
out vec4 c_fragColor;

in vec2 texCoord;
in vec3 normal;
in vec3 worldPos;

uniform sampler2D c_mainTexture;

#include "buildin/shaders/include/light.inc"

void main() {
	vec4 albedo = texture(c_mainTexture, texCoord);
	c_fragColor = albedo * vec4(calculateDirectionalLight(worldPos, normalize(normal)), 1);
}
