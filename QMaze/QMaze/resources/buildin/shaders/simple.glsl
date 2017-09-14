#version 330 core

#shader vert

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 vertexPosition_modelspace;

void main(){
    gl_Position.xyz = vertexPosition_modelspace;
    gl_Position.w = 1.0;
}

#shader frag

// Ouput data
out vec3 color;

void main()
{
	// Output color = red 
	color = vec3(1,0,0);
}

