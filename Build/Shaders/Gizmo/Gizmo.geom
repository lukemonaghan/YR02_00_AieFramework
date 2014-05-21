#version 330
// Gizmo Geometry Shader

layout (points) in;
layout (triangle_strip,max_vertices = 512) out;

in vec3 vPosition[];
in vec3 vInformation[];
in vec4 vColour[];
in int  vID[];
in int  vSmooth[];
in int  vFacing[];

out vec4 gColour;

uniform mat4 ViewProjection;

// CUBE CONSTANTS
const vec4 CUBEvertices[8] = vec4[8](
	vec4( -1.0f, -1.0f,  1.0f, 1.0f),	//A 0
	vec4(  1.0f, -1.0f,  1.0f, 1.0f),	//B 1
	vec4( -1.0f,  1.0f,  1.0f, 1.0f),	//C 2
	vec4(  1.0f,  1.0f,  1.0f, 1.0f),	//D 3
	vec4( -1.0f, -1.0f,	-1.0f, 1.0f),	//F 4
	vec4(  1.0f, -1.0f,	-1.0f, 1.0f),	//G 5
	vec4( -1.0f,  1.0f,	-1.0f, 1.0f),	//E 6
	vec4(  1.0f,  1.0f,	-1.0f, 1.0f)	//H 7
	);
const int CUBEindices[24] = int[24](0, 1, 2, 3, 1, 5, 3, 7, 5, 4, 7, 6, 4, 0, 6, 2, 2, 3, 6, 7, 4, 5, 0, 1);
const vec3 CUBEtexcoords[8] = vec3[8](vec3(0,1,0), vec3(1,1,0), vec3(0,0,0), vec3(1,0,0), vec3(1,1,0), vec3(1,1,0), vec3(1,1,0), vec3(1,1,0));

void DrawCube(){
	for (int i = 0;i < 24;i++){
		vec4 Pos = vec4(vPosition[0],1.0f);
		vec4 Extents = vec4(vInformation[0],0.0f);
		gl_Position = ViewProjection * (Pos + (Extents * CUBEvertices[CUBEindices[i]]));
		EmitVertex();
		if (int(mod(i,4)) == 3) { EndPrimitive(); }
	}
}

// CUBE CONSTANTS

// PLANE CONSTANTS
const vec4 PLANEvertices[4] = vec4[4](
	vec4( -1.0f, 0.0f, -1.0f, 1.0f),	//A 0
	vec4( -1.0f, 0.0f,  1.0f, 1.0f),	//B 1
	vec4(  1.0f, 0.0f,  1.0f, 1.0f),	//C 2
	vec4(  1.0f, 0.0f, -1.0f, 1.0f) 	//D 3
	);
const int PLANEindices[6] = int[6](0, 1, 2, 0, 2, 3);
const vec2 PLANEtexcoords[4] = vec2[4](vec2(0,1), vec2(1,1), vec2(0,0), vec2(1,0));

void DrawPlane(){
	for (int i = 0;i < 6;i++){
		vec4 Pos = vec4(vPosition[0],1.0f);
		vec4 Up = vec4(vInformation[0],0.0f);
		gl_Position = ViewProjection * (Pos + (Up * PLANEvertices[PLANEindices[i]]));
		EmitVertex();
		if (int(mod(i,3)) == 2) { EndPrimitive(); }
	}
}
// PLANE CONSTANTS

// POINT CONSTANTS
// Not actually a point, really small box
void DrawPoint(){
	for (int i = 0;i < 24;i++){
		vec4 Pos = vec4(vPosition[0],1.0f);
		vec4 Extents = vec4(0.01f,0.01f,0.01f,0.0f);
		gl_Position = ViewProjection * (Pos + (Extents * CUBEvertices[CUBEindices[i]]));
		EmitVertex();
		if (int(mod(i,4)) == 3) { EndPrimitive(); }
	}
}
// POINT CONSTANTS

void main(){
	gColour = vColour[0];
	switch(vID[0]){
		case 0:
			DrawPoint();
			break;
		case 1:
			DrawCube();
			break;
		case 2:
			DrawPlane();
			break;
	}
}

/*
	gl_Position = ViewProjection * Position;
	EmitVertex();
	EndPrimitive();
*/

