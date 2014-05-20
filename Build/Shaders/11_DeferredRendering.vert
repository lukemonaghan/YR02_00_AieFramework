#version 330

in vec4 Position;
in vec4 Normal;
in vec2 TexCoord;

out vec2 texCoord;
out vec3 normal;
out vec4 wPos;

uniform mat4 Model; 
uniform mat4 View;
uniform mat4 Projection;

void main() {
	texCoord = TexCoord;
	normal = normalize((Model * Normal).xyz);
	wPos = (Model * Position);
	gl_Position = Projection * View * Model * Position;
}
