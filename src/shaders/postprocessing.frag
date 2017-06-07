#version 330 core

in vec2 TexCoords;
out vec4 fragColor;

#define NUM 9
uniform sampler2D image;
uniform vec2 offsets[NUM];
uniform int edge_kernel[NUM];
uniform float blur_kernel[NUM];

uniform bool shake;
uniform bool confuse;
uniform bool chaos;

void main(){
	vec3 sample[NUM];
	if(chaos || shake){
		for(int i = 0; i < NUM; i++){
			sample[i] = vec3(texture(image, TexCoords + offsets[i]));
		}
	}
	
	if (chaos){
		for(int i = 0; i < NUM; i++){
			fragColor += vec4(sample[i] * edge_kernel[i], 0.0);
		}
		fragColor.a = 1.0;
	} else if (confuse){
		fragColor = vec4(1.0 - texture(image, TexCoords).rgb, 1.0);
	} else if(shake){
		for(int i = 0; i < NUM; i++){
			fragColor += vec4(sample[i] * blur_kernel[i], 0.0);
		}
		fragColor.a = 1.0;
	} else{
		fragColor = texture(image, TexCoords);
	}
}