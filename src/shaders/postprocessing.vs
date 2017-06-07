#version 330 core

layout (location = 0) in vec4 vertex; // <vec2 position, vec2 texCoords>

out vec2 TexCoords;

uniform bool shake;
uniform float time;
uniform bool confuse;
uniform bool chaos;

void main(){	
	gl_Position = vec4(vertex.xy, 0.0, 1.0);
	TexCoords = vertex.zw;
	if (chaos){
		float strength = 0.3;
		TexCoords = vec2(TexCoords.x + sin(time) * strength, TexCoords.y + cos(time) * strength);
	} else if (confuse){
		TexCoords = vec2(1.0 - TexCoords.x, 1.0 - TexCoords.y);
	}
	
	if (shake){
		float strength = 0.01;
		gl_Position.x += sin(time * 10) * strength;
		gl_Position.y += cos(time * 15) * strength;
	}
}