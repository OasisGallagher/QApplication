#version 330

#shader vert

in vec3 pos;
in vec3 texCoord;
in vec3 normal;

uniform mat4 MV;
uniform mat4 MVP;

out VertOutput {
	vec3 texCoord;
	vec3 normal;
	vec3 worldPos;	// camera space.
} vertOutput;

void main() {
	gl_Position = MVP * vec4(pos, 1);
	
	vertOutput.texCoord = texCoord;
	vertOutput.normal = (MV * vec4(normal, 0)).xyz; 
	vertOutput.worldPos = (MV * vec4(pos, 1)).xyz;
}

#shader frag

uniform samplerCube cube; 

in VertOutput {
	vec3 texCoord;
	vec3 normal;
	vec3 worldPos;
} fragInput;

out vec4 fragColor;

void main() {
	vec3 dir = reflect(fragInput.worldPos, fragInput.normal);
	fragColor = texture(cube, dir);
}
