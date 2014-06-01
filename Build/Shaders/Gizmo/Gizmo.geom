#version 330
// Gizmo Geometry Shader

layout (points) in;
layout (triangle_strip,max_vertices = 1024) out;
// MaxVerts for a 660ti is 1024
// Dividing this by our two vec4's (gl_Position and gColour)
// we are left with 128 outputs

in mat4 vModel[];
in vec3 vPosition[];
in vec3 vInformation[];
in vec4 vColour[];
in int  vID[];
in int  vSmooth[];
in int  vFacing[];

//out vec4 gColour;

uniform mat4 ViewProjection;

// BOX CONSTANTS
const vec4 BOXvertices[8] = vec4[8](
	vec4( -1.0f, -1.0f,  1.0f, 1.0f),	//A 0
	vec4(  1.0f, -1.0f,  1.0f, 1.0f),	//B 1
	vec4( -1.0f,  1.0f,  1.0f, 1.0f),	//C 2
	vec4(  1.0f,  1.0f,  1.0f, 1.0f),	//D 3
	vec4( -1.0f, -1.0f,	-1.0f, 1.0f),	//F 4
	vec4(  1.0f, -1.0f,	-1.0f, 1.0f),	//G 5
	vec4( -1.0f,  1.0f,	-1.0f, 1.0f),	//E 6
	vec4(  1.0f,  1.0f,	-1.0f, 1.0f)	//H 7
	);
const int BOXindices[24] = int[24](0, 1, 2, 3, 1, 5, 3, 7, 5, 4, 7, 6, 4, 0, 6, 2, 2, 3, 6, 7, 4, 5, 0, 1);
const vec3 BOXtexcoords[8] = vec3[8](vec3(0,1,0), vec3(1,1,0), vec3(0,0,0), vec3(1,0,0), vec3(1,1,0), vec3(1,1,0), vec3(1,1,0), vec3(1,1,0));
void DrawBox(){
	vec4 Pos = vec4(vPosition[0],1.0f);
	vec4 Extents = vec4(vInformation[0],0.0f);
	for (int i = 0;i < 24;i++){
		gl_Position = ViewProjection * vModel[0] * (Pos + (Extents * BOXvertices[BOXindices[i]]));
		EmitVertex();
		if (int(mod(i,4)) == 3) { EndPrimitive(); }
	}
}
// BOX CONSTANTS

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
	vec4 Pos = vec4(vPosition[0],1.0f);
	vec4 Up = vec4(vInformation[0],0.0f);
	for (int i = 0;i < 6;i++){
		gl_Position = ViewProjection * vModel[0] * (Pos + (Up * PLANEvertices[PLANEindices[i]]));
		EmitVertex();
		if (int(mod(i,3)) == 2) { EndPrimitive(); }
	}
}
// PLANE CONSTANTS

// POINT CONSTANTS
void DrawPoint(){
	vec4 Pos = vec4(vPosition[0],1.0f);
	vec4 Extents = vec4(0.01f,0.01f,0.01f,0.0f);
	for (int i = 0;i < 24;i++){
		gl_Position = ViewProjection * vModel[0] * (Pos + (Extents * BOXvertices[BOXindices[i]]));
		EmitVertex();
		if (int(mod(i,4)) == 3) { EndPrimitive(); }
	}
}
// POINT CONSTANTS

// SPHERE CONSTANTS
const vec4 SPHEREvertices[26] = vec4[26](
	vec4( 0.0f, 1.0f, 0.0f, 1.0f), //TOP 0

	vec4(-0.5f, 0.7f, 0.0f, 1.0f), //T1 1
	vec4(-0.3f, 0.7f, 0.3f, 1.0f), //T2 2
	vec4( 0.0f, 0.7f, 0.5f, 1.0f), //T3 3
	vec4( 0.3f, 0.7f, 0.3f, 1.0f), //T4 4
	vec4( 0.5f, 0.7f, 0.0f, 1.0f), //T5 5
	vec4( 0.3f, 0.7f,-0.3f, 1.0f), //T6 6
	vec4( 0.0f, 0.7f,-0.5f, 1.0f), //T7 7
	vec4(-0.3f, 0.7f,-0.3f, 1.0f), //T8 8

	vec4(-1.0f, 0.0f, 0.0f, 1.0f), //M1 9
	vec4(-0.7f, 0.0f, 0.7f, 1.0f), //M2 10
	vec4( 0.0f, 0.0f, 1.0f, 1.0f), //M3 11
	vec4( 0.7f, 0.0f, 0.7f, 1.0f), //M4 12
	vec4( 1.0f, 0.0f, 0.0f, 1.0f), //M5 13
	vec4( 0.7f, 0.0f,-0.7f, 1.0f), //M6 14
	vec4( 0.0f, 0.0f,-1.0f, 1.0f), //M7 15
	vec4(-0.7f, 0.0f,-0.7f, 1.0f), //M8 16

	vec4(-0.5f,-0.7f, 0.0f, 1.0f), //b1 17
	vec4(-0.3f,-0.7f, 0.3f, 1.0f), //b2 18
	vec4( 0.0f,-0.7f, 0.5f, 1.0f), //b3 19
	vec4( 0.3f,-0.7f, 0.3f, 1.0f), //b4 20
	vec4( 0.5f,-0.7f, 0.0f, 1.0f), //b5 21
	vec4( 0.3f,-0.7f,-0.3f, 1.0f), //b6 22
	vec4( 0.0f,-0.7f,-0.5f, 1.0f), //b7 23
	vec4(-0.3f,-0.7f,-0.3f, 1.0f), //b8 24

	vec4( 0.0f,-1.0f, 0.0f, 1.0f)  //BOTTOM 25
	);
const int SphereIndCount = 72;
const int SPHERETopindices[SphereIndCount] = int[SphereIndCount](
	0, 1, 2, 0, 2, 3, 0, 3, 4, 0, 4, 5, 0, 5, 6, 0, 6, 7, 0, 7, 8, 0, 8, 1,
	2, 1, 9 , 2, 9, 10,
	3, 2, 10, 3 ,10 ,11,
	4, 3, 11, 4, 11, 12,
	5, 4, 12, 5, 12, 13,
	6, 5, 13, 6, 13, 14,
	7, 6, 14, 7, 14, 15,
	8, 7, 15, 8, 15, 16,
	1, 8, 16, 1, 16, 9
	);
const int SPHEREBottomindices[SphereIndCount] = int[SphereIndCount](
	10,  9, 17, 10, 17, 18,
	11, 10, 18, 11, 18 ,19,
	12, 11, 19, 12, 19, 20,
	13, 12, 20, 13, 20, 21,
	14, 13, 21, 14, 21, 22,
	15, 14, 22, 15, 22, 23,
	16, 15, 23, 16, 23, 24,
	 9, 16, 24,  9, 24, 17, 
	25, 18, 17, 25, 19, 18, 25, 20, 19, 25, 21, 20, 25, 22, 21, 25, 23, 22, 25, 24, 23, 25, 17, 24
	);
void DrawSphere(){
	vec4 Pos = vec4(vPosition[0],1.0f);
	vec4 Radius = vec4(vInformation[0].xxx,0.0f);
	for (int i = 0;i < SphereIndCount; i++){
		gl_Position = ViewProjection * vModel[0] * (Pos + (Radius * SPHEREvertices[SPHERETopindices[i]]));
		EmitVertex();
		if (int(mod(i,3)) == 2) { EndPrimitive(); }
	}
	for (int i = 0;i < SphereIndCount; i++){
		gl_Position = ViewProjection * vModel[0] * (Pos + (Radius * SPHEREvertices[SPHEREBottomindices[i]]));
		EmitVertex();
		if (int(mod(i,3)) == 2) { EndPrimitive(); }
	}
}
// SPHERE CONSTANTS

// CYLINDER CONSTANTS
void DrawCylinder(){
	vec4 Pos = vec4(vPosition[0],1.0f);
	vec4 Extents = vec4(0.01f,0.01f,0.01f,0.0f);
	for (int i = 0;i < 24;i++){
		gl_Position = ViewProjection * vModel[0] * (Pos + (Extents * BOXvertices[BOXindices[i]]));
		EmitVertex();
		if (int(mod(i,4)) == 3) { EndPrimitive(); }
	}
}
// CYLINDER CONSTANTS

// RING CONSTANTS
void DrawRing(){
	for (int i = 0;i < 24;i++){
		vec4 Pos = vec4(vPosition[0],1.0f);
		vec4 Extents = vec4(0.01f,0.01f,0.01f,0.0f);
		gl_Position = ViewProjection * vModel[0] * (Pos + (Extents * BOXvertices[BOXindices[i]]));
		EmitVertex();
		if (int(mod(i,4)) == 3) { EndPrimitive(); }
	}
}
// RING CONSTANTS

// DISK CONSTANTS
const vec4 DISKvertices[18] = vec4[18](
	vec4( 0.0f, 0.1f, 0.0f, 1.0f), //TOP 0

	vec4(-1.0f, 0.1f, 0.0f, 1.0f), // M1 1
	vec4(-0.7f, 0.1f, 0.7f, 1.0f), // M2 2
	vec4( 0.0f, 0.1f, 1.0f, 1.0f), // M3 3
	vec4( 0.7f, 0.1f, 0.7f, 1.0f), // M4 4
 
	vec4( 1.0f, 0.1f, 0.0f, 1.0f), // M5 5
	vec4( 0.7f, 0.1f,-0.7f, 1.0f), // M6 6
	vec4( 0.0f, 0.1f,-1.0f, 1.0f), // M7 7
	vec4(-0.7f, 0.1f,-0.7f, 1.0f), // M8 8
 
	vec4(-1.0f,-0.1f, 0.0f, 1.0f), // M1 9
	vec4(-0.7f,-0.1f, 0.7f, 1.0f), // M2 10
	vec4( 0.0f,-0.1f, 1.0f, 1.0f), // M3 11
	vec4( 0.7f,-0.1f, 0.7f, 1.0f), // M4 12
 
	vec4( 1.0f,-0.1f, 0.0f, 1.0f), // M5 13
	vec4( 0.7f,-0.1f,-0.7f, 1.0f), // M6 14
	vec4( 0.0f,-0.1f,-1.0f, 1.0f), // M7 15
	vec4(-0.7f,-0.1f,-0.7f, 1.0f), // M8 16

	vec4( 0.0f,-0.1f, 0.0f, 1.0f)  //BOTTOM 17
	);
const int DISKIndCount = 54;
const int DISKindices[DISKIndCount] = int[DISKIndCount](
	0, 1, 2, 0, 2, 3, 0, 3, 4, 0, 4, 5, 0, 5, 6, 0, 6, 7, 0, 7, 8, 0, 8, 1,
	2, 1, 9, 2, 9, 10,
	17, 10, 9, 17, 11, 10, 17, 12, 11, 17, 13, 12, 17, 14, 13, 17, 15, 14, 17, 16, 15, 17, 9, 16
	);
void DrawDisk(){
	vec4 Pos = vec4(vPosition[0],1.0f);
	vec4 Radius = vec4(vInformation[0].xxx,0.0f);
	for (int i = 0;i < DISKIndCount; i++){
		gl_Position = ViewProjection * vModel[0] * (Pos + (Radius * DISKvertices[DISKindices[i]]));
		EmitVertex();
		if (int(mod(i,3)) == 2) { EndPrimitive(); }
	}
}
// DISK CONSTANTS

// CONE CONSTANTS
const vec4 CONEvertices[10] = vec4[10](
	vec4( 0.0f, 1.0f, 0.0f, 1.0f), //TOP 0

	vec4(-1.0f, 0.0f, 0.0f, 1.0f), //M1 1
	vec4(-0.7f, 0.0f, 0.7f, 1.0f), //M2 2
	vec4( 0.0f, 0.0f, 1.0f, 1.0f), //M3 3
	vec4( 0.7f, 0.0f, 0.7f, 1.0f), //M4 4

	vec4( 1.0f, 0.0f, 0.0f, 1.0f), //M5 5
	vec4( 0.7f, 0.0f,-0.7f, 1.0f), //M6 6
	vec4( 0.0f, 0.0f,-1.0f, 1.0f), //M7 7
	vec4(-0.7f, 0.0f,-0.7f, 1.0f), //M8 8

	vec4( 0.0f,0.0f, 0.0f, 1.0f)  //BOTTOM 9
	);
const int CONEIndCount = 48;
const int CONEindices[CONEIndCount] = int[CONEIndCount](
	0, 1, 2, 0, 2, 3, 0, 3, 4, 0, 4, 5, 0, 5, 6, 0, 6, 7, 0, 7, 8, 0, 8, 1,
	9, 2, 1, 9, 3, 2, 9, 4, 3, 9, 5, 4, 9, 6, 5, 9, 7, 6, 9, 8, 7, 9, 1, 8
	);
void DrawCone(){
	vec4 Pos = vec4(vPosition[0],1.0f);
	vec4 Radius = vec4(vInformation[0].xxx,0.0f);
	for (int i = 0;i < CONEIndCount; i++){
		gl_Position = ViewProjection * vModel[0] * (Pos + (Radius * CONEvertices[CONEindices[i]]));
		EmitVertex();
		if (int(mod(i,3)) == 2) { EndPrimitive(); }
	}
}
// CONE CONSTANTS

void main(){
	//gColour = vColour[0];
	switch(vID[0]){
		case 0:
			DrawPoint();
			break;
		case 1:
			DrawBox();
			break;
		case 2:
			DrawPlane();
			break;
		case 3:
			DrawSphere();
			break;
		case 4:
			DrawCylinder();
			break;
		case 5:
			DrawRing();
			break;
		case 6:
			DrawDisk();
			break;
		case 7:
			DrawCone();
			break;
	}
}

