#version 150 
 
in vec4 Position;
in vec4 Color;
in vec2 TexCoord1;

out vec2 VS_UV;
out vec4 VS_Color;

uniform mat4 Model = mat4(1);;
uniform mat4 View = mat4(1);
uniform mat4 Projection = mat4(1);

uniform float Time = 1;

uniform float WindPower = 0.1f;

void main() {
	VS_UV     = TexCoord1;
	VS_Color  = Color;

	vec4 pos = Position;
	//only move top verts, kinda hacky but whatevs
	if (Position.y != 0.0f && View != mat4(1)){
		pos.xz += sin(Time * 3.14159 ) * cos((Model * pos).x + (Model * pos).z ) * WindPower;
		pos.zx += cos(Time * 3.14159 ) * cos((Model * pos).x + (Model * pos).z ) * WindPower;
	}

	gl_Position = Projection * View * Model * pos;
}