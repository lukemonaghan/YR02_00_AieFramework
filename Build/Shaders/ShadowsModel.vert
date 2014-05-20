#version 330

layout (location = 0 )in vec4 Position;
layout (location = 2 )in vec4 Normals;
layout (location = 7 )in vec2 TexCoord;

out vec4 normals;
out vec2 texCoord;
out vec4 shadowCoord;

uniform mat4 projectionView;
uniform mat4 world;

uniform mat4 lightProjectionView;

const mat4 SHADOW_BIAS_MATRIX = mat4(
					0.5, 0.0, 0.0, 0.0,
					0.0, 0.5, 0.0, 0.0,
					0.0, 0.0, 0.5, 0.0,
					0.5, 0.5, 0.5, 1.0
);

void main() {
	normals = world * Normals;
	texCoord = TexCoord;
	shadowCoord = SHADOW_BIAS_MATRIX * lightProjectionView * world * Position;
	gl_Position = projectionView * world * Position;
}