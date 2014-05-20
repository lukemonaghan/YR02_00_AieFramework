#version 150

in vec4 VS_Position;
in vec4 VS_Colour;
in vec2 VS_TexCoord1;

out vec4 OutColour;

uniform sampler2D DiffuseTexture;

void main() {
	OutColour = texture( DiffuseTexture, VS_TexCoord1 ) * VS_Colour;
	OutColour.r += min( VS_Position.y * 0.1f , 0.1f);
	OutColour.g += min( VS_Position.y * 0.7f , 0.2f);
	OutColour.b += min( VS_Position.y * 1.0f , 0.4f);
	OutColour.a += min( VS_Position.y * 0.2f , 0.4f);
}