#version 330

in vec4 position;
in vec4 normal;

out vec4 viewPosition;
out vec4 viewNormal;

uniform mat4 Projection;
uniform mat4 View;
uniform mat4 Model;

void main() {
	mat4 InvModel = inverse(Model);
	viewPosition = (View * Model) * position;
	viewNormal = View * InvModel * normal;
	gl_Position = Projection * viewPosition;
}