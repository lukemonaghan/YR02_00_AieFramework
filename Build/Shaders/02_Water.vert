#version 150

in vec4 Position;
in vec4 Colour;
in vec2 TexCoord1;

out vec4 VS_Position;
out vec4 VS_Colour;
out vec2 VS_TexCoord1;

uniform mat4 Projection;
uniform mat4 View;
uniform float Time;
uniform float Height;

uniform float WaveLength = 0.2f;

const float PI = 3.14159265f;

void main() { 
	float speed = 0.5f;
	float magnitude = (sin((Time * PI / ((256.0f) * speed))) * 0.01f + WaveLength);
	//Passing to Fragment
	VS_Position = Position;
	//large generic waves
	VS_Position.x += cos(( Time * PI * speed) + (Position.z - Position.y) * (PI*WaveLength*normalize(vec2(Time*2,Time*4)))) * magnitude * Height;
	VS_Position.y += sin(( Time * PI * speed) + (Position.z + Position.x) * (PI*WaveLength)) * magnitude;
	//small waves
	VS_Position.y += sin(( Time * PI * speed) + (Position.x + Position.z) * (PI*WaveLength*normalize(vec2(Time*4,Time*8)))) * magnitude * Height * 0.5f;
	VS_Position.y += sin(( Time * PI * speed) + (Position.x - Position.z) * (PI*WaveLength*normalize(vec2(Time*2,Time*4)))) * magnitude * Height * 0.5f;
	//ripple waves
	VS_Position.y += cos(( Time * PI * speed) + (Position.x + Position.z) * (PI*WaveLength*8)) * magnitude * (Height * 0.1f);
	VS_Position.y += cos(( Time * PI * speed) + (Position.x - Position.z) * (PI*WaveLength*8)) * magnitude * (Height * 0.1f);
	VS_Colour = Colour * vec4(0.0f,0.2f,1.0f,0.4f);
	VS_TexCoord1 = TexCoord1;
	//OpenGL stuff
	gl_Position = Projection * View * VS_Position;
}