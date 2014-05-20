 #version 330

in vec4 Position;
in vec4 Colour;
in vec2 TexCoord1;

out vec4 VS_Colour;
out vec2 VS_TexCoord1;

uniform mat4 Projection;
uniform mat4 View;

void main() {
	gl_Position = Projection * View * Position;
	VS_TexCoord1 = TexCoord1;
	VS_Colour = Colour;
}
