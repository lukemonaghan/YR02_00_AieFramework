#version 330
// Gizmo Geometry Shader

layout (points) in;
layout (triangle_strip,max_vertices = 1024) out;

in mat4 vModel[];
in vec3 vPosition[];
in vec3 vInformation[];
in vec4 vColour[];
in int  vID[];
in int  vSmooth[];
in int  vFacing[];

out vec4 gColour;

uniform mat4 ViewProjection;

void addTri(vec4 Pos1, vec4 Pos2, vec4 Pos3){
	gl_Position = ViewProjection * Pos1;
	EmitVertex();
	gl_Position = ViewProjection * Pos2;
	EmitVertex();
	gl_Position = ViewProjection * Pos3;
	EmitVertex();
	EndPrimitive();
}

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
	for (int i = 0;i < 24;i++){
		vec4 Pos = vec4(vPosition[0],1.0f);
		vec4 Extents = vec4(vInformation[0],0.0f);
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
	for (int i = 0;i < 6;i++){
		vec4 Pos = vec4(vPosition[0],1.0f);
		vec4 Up = vec4(vInformation[0],0.0f);
		gl_Position = ViewProjection * vModel[0] * (Pos + (Up * PLANEvertices[PLANEindices[i]]));
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
		gl_Position = ViewProjection * vModel[0] * (Pos + (Extents * BOXvertices[BOXindices[i]]));
		EmitVertex();
		if (int(mod(i,4)) == 3) { EndPrimitive(); }
	}
}
// POINT CONSTANTS

float _PI = 3.14159;

// SPHERE CONSTANTS
void DrawSphere(){
	
	vec4 a_center = vec4(vPosition[0],1.0f);

	float a_latMax = 90;
	float a_latMin = -90;
	float a_longMax = 360;
	float a_longMin = 0;

	float a_radius = vInformation[0].x;
	float a_rows = vInformation[0].y;
	float a_columns = vInformation[0].z;
	int ia_columns = int(a_columns);

	float inverseRadius = 1/a_radius;
	float inverseRows = 1/a_rows;
	float inverseColumns = 1/a_columns;

	float DEG2RAD = _PI / 180;

	//Lets put everything in radians first
	float latitiudinalRange = (a_latMax - a_latMin) * DEG2RAD;
	float longitudinalRange = (a_longMax - a_longMin) * DEG2RAD;

	vec4 v4Array[512];

	int count = 0;
	for (int row = 0; row <= a_rows; ++row){
		// y ordinates this may be a little confusing but here we are navigating around the xAxis in GL
		float ratioAroundXAxis = float(row) * inverseRows;
		float radiansAboutXAxis  = ratioAroundXAxis * latitiudinalRange + (a_latMin * DEG2RAD);
		float y  =  a_radius * sin(radiansAboutXAxis);
		float z  =  a_radius * cos(radiansAboutXAxis);
		
		for ( int col = 0; col <= a_columns; ++col ){
			float ratioAroundYAxis   = float(col) * inverseColumns;
			float theta = ratioAroundYAxis * longitudinalRange + (a_longMin * DEG2RAD);
			vec4 v4Point = vec4( -z * sin(theta), y, -z * cos(theta),0.0f );
			vec4 v4Normal = vec4( inverseRadius * v4Point.x, inverseRadius * v4Point.y, inverseRadius * v4Point.z,0.0f);

			v4Point = vModel[0] * v4Point;
			v4Normal = (vModel[0] * v4Normal);

			int index = int(row * a_columns + (col % ia_columns));
			v4Array[index] = v4Point;
			if (index >= count){
				count = index;
			}
		}
	}

	for (int face = 0; face < 126; face+=3){
		addTri( a_center + v4Array[face], a_center + v4Array[face+1], a_center + v4Array[face+2]);
	}
	
	//for (int face = 0; face < count; face++){
	//	int iNextFace = face + 1;		
	//	
	//	if( iNextFace % ia_columns == 0 ){
	//		iNextFace = iNextFace - (ia_columns);
	//	}
	//
	//	if( face % ia_columns == 0 && longitudinalRange < (_PI * 2)){
	//		continue;
	//	}
	//
	//	addTri( a_center + v4Array[iNextFace+ia_columns], a_center + v4Array[face], a_center + v4Array[iNextFace]);
	//	addTri( a_center + v4Array[iNextFace+ia_columns], a_center + v4Array[face+ia_columns], a_center + v4Array[face]);		
	//}
}
// SPHERE CONSTANTS

// CYLINDER CONSTANTS
void DrawCylinder(){
	for (int i = 0;i < 24;i++){
		vec4 Pos = vec4(vPosition[0],1.0f);
		vec4 Extents = vec4(0.01f,0.01f,0.01f,0.0f);
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
void DrawDisk(){
	for (int i = 0;i < 24;i++){
		vec4 Pos = vec4(vPosition[0],1.0f);
		vec4 Extents = vec4(0.01f,0.01f,0.01f,0.0f);
		gl_Position = ViewProjection * vModel[0] * (Pos + (Extents * BOXvertices[BOXindices[i]]));
		EmitVertex();
		if (int(mod(i,4)) == 3) { EndPrimitive(); }
	}
}
// DISK CONSTANTS

void main(){
	gColour = vColour[0];
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
	}
}

/*
	gl_Position = ViewProjection * Position;
	EmitVertex();
	EndPrimitive();
*/

