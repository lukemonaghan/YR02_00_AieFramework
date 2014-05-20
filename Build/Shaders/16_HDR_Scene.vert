#version 330

in vec4 position;
in vec4 normal;
in vec4 tangent;
in vec4 biNormal;
in vec2 texCoord;

out vec4 worldPosition;
out vec4 worldNormal;
out vec4 worldTangent;
out vec4 worldBiNormal;
out vec2 textureCoord;

uniform mat4 Model;
uniform mat4 Projection;
uniform mat4 View;

void main() {
	textureCoord = texCoord;

	mat4 InvModel = inverse(Model);

	worldPosition = Model * position;

	worldNormal = InvModel * normal;
	worldTangent = InvModel * tangent;
	worldBiNormal = InvModel * biNormal;

	gl_Position = Projection * View * worldPosition;
}

