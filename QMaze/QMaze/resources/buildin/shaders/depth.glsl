#version 330

#shader vert

in vec3 c_position;

uniform mat4 c_cc;

void main() {
	gl_Position = c_cc * vec4(c_position, 1);
}

#shader frag

out float depth;

void main() {
	depth = gl_FragCoord.z;
}

