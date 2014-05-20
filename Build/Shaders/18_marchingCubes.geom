#version 330

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

in vec3 Position[];

out vec3 Normal;
out vec3 position;

uniform mat4 Projection;
uniform mat4 View;
uniform mat4 Model;

void main() {
	mat4 InvModel = inverse(Model);
	Normal = (Model * vec4(normalize(cross(Position[2] - Position[1],Position[0] - Position[1])),0)).xyz;

	gl_Position = Projection * View * Model * vec4(Position[0],1);
	position = gl_Position.xyz;
	EmitVertex();
	
	gl_Position = Projection * View * Model * vec4(Position[1],1);
	position = gl_Position.xyz;
	EmitVertex();

	gl_Position = Projection * View * Model * vec4(Position[2],1);
	position = gl_Position.xyz;
	EmitVertex();
}