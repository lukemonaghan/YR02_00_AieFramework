#version 150

layout (points) in;
layout (triangle_strip,max_vertices = 24) out;

in vec4 VSPosition[];

out vec3 GSTexCoord1;

uniform mat4 Projection;
uniform mat4 View;

vec4 cubeVerts[8] = vec4[8](vec4(-0.5,-0.5, 0.5,0),	vec4( 0.5,-0.5, 0.5,0),	vec4(-0.5, 0.5, 0.5,0),	vec4( 0.5, 0.5, 0.5,0),vec4(-0.5,-0.5,-0.5,0),	vec4( 0.5,-0.5,-0.5,0),	vec4(-0.5, 0.5,-0.5,0),	vec4( 0.5, 0.5,-0.5,0));
//int ind[14] = int[14](0, 1, 2, 3, 7, 1, 5, 4, 7, 6, 2, 4, 0, 1);

int ind[24] = int[24](
	0,1,2,3,
	1,5,3,7,
	5,4,7,6,
	4,0,6,2,
	2,3,6,7,
	4,5,0,1
	);

void UV3D(int i){
	int face = int(mod(i,4));
	//GSTexCoord1 = (VSPosition[0].xyz + cubeVerts[ind[i]].xyz - vec3(0,8,0));
	//return;
	//GSTexCoord1 = vec4(i/14.0,i/14.0,i/14.0,1);
	vec3 Pos = VSPosition[0].xyz - vec3(-0.5,7.5,-0.5);
	switch(face){
		case  0:Pos.x = 1;break;
		case  1:Pos.y = 1;break;
		case  2:Pos.z = 1;break;
		case  3:Pos.x = 1;break;
		case  4:Pos.y = 1;break;
		case  5:Pos.z = 1;break;
	}
	GSTexCoord1 = (Pos + cubeVerts[ind[i]].xyz);
}

void main(){
	for (int i = 0; i < 24; i++){
		gl_Position = Projection * View * (VSPosition[0] + cubeVerts[ind[i]]);
		UV3D(i);
		EmitVertex();
		if (mod(i,4) == 3){EndPrimitive();}
	}
}
