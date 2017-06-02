#version 330 core

layout (location = 0) in vec4 vertex; // <vec2 position, vec2 texCoords>

out vec2 TexCoords;

uniform mat4 projection;
uniform vec2 offset;

void main(){
	TexCoords = vertex.zw;
	float scale = 10.0;	
	gl_Position = projection * vec4(vertex.xy * scale + offset, 0.0, 1.0);
}