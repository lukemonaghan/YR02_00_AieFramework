#version 330

in vec4 Position;
in vec4 Colour;
in vec4 Normal;

out vec4 VS_Colour;
out vec4 VS_Normal;
out vec4 VS_Position;

uniform mat4 Model; 
uniform mat4 View;
uniform mat4 Projection;

void main() {
	VS_Colour = Colour;
	VS_Normal = normalize((Model * Normal));
	VS_Position = (Model * Position);
	gl_Position = Projection * View * Model * Position;
}
