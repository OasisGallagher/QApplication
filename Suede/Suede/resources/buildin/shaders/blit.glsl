#shader vertex
#include "shaders/include/posteffectvertexshader.inc"

#shader fragment
out vec4 c_fragColor;

in vec2 texCoord;

uniform sampler2D c_mainTexture;

void main() {
	c_fragColor = texture(c_mainTexture, texCoord);
}
