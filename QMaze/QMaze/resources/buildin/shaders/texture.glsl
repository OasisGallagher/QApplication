#version 330

#shader vert
in vec3 c_position;
in vec2 c_texCoord;

out vec2 texCoord;

uniform mat4 c_modelToClipSpaceMatrix;

void main() {
	texCoord = c_texCoord;
	gl_Position = c_modelToClipSpaceMatrix * vec4(c_position, 1);
}

#shader frag
in vec2 texCoord;

out vec3 color;

uniform sampler2D c_diffuseTexture;

void main() {
	color = texture(c_diffuseTexture, texCoord).rgb;
}

