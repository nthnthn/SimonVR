#version 330 core

// This is an example vertex shader. GLSL is very similar to C.
// You can define extra functions if needed, and the main() function is
// called when the vertex shader gets run.
// The vertex shader gets called once per vertex.

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;

uniform mat4 MVP;
uniform mat4 toWorld;

out vec3 fragPos;
out vec3 norm;
out vec3 pos;

void main()
{
    gl_Position = MVP * vec4(position.x, position.y, position.z, 1.0);
	norm =  normalize(mat3(transpose(inverse(toWorld))) * normal);
	pos = position;
	fragPos = vec3(toWorld * vec4(position, 1.0f));
}