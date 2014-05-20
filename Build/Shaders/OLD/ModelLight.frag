#version 330

in vec2 vUV;
in vec4 vColor;
in vec4 vPosition;

out vec4 outColor;

uniform sampler2D DiffuseTexture;
uniform sampler2D SpecularTexture;
uniform sampler2D NormalTexture;

uniform vec3 LightPosition[3];
uniform float LightDistance[3];
uniform vec4 LightColour[3];

float GetDistance(vec3 a, vec3 b){
	return sqrt(((a.x - b.x)*(a.x - b.x)) + ((a.y - b.y)*(a.y - b.y)) + ((a.z - b.z)*(a.z - b.z))); 
}

void main() { 
	//Specular Mapping
	//vec4 Spec = texture2D( SpecularTexture, vUV );
	//float Light = dot(LightPosition,Spec.xyz);//
	vec4 Light1 = vec4((LightDistance[0] - (GetDistance(vPosition.xyz,LightPosition[0]) / LightDistance[0])) * LightColour[0].rgb,1.0f) * LightColour[0].a;
	vec4 Light2 = vec4((LightDistance[1] - (GetDistance(vPosition.xyz,LightPosition[1]) / LightDistance[1])) * LightColour[1].rgb,1.0f) * LightColour[1].a;
	vec4 Light3 = vec4((LightDistance[2] - (GetDistance(vPosition.xyz,LightPosition[2]) / LightDistance[2])) * LightColour[2].rgb,1.0f) * LightColour[2].a;

	vec4 Light = normalize(Light1 + Light2 + Light3);
	//Normal Mapping
	vec4 Norm = normalize(texture2D( NormalTexture, vUV )*2.0 - 1.0);
	float NormVal1 = dot(LightPosition[0],Norm.xyz);
	float NormVal2 = dot(LightPosition[1],Norm.xyz);
	float NormVal3 = dot(LightPosition[2],Norm.xyz);
	float NormVal = (NormVal1 + NormVal2 + NormVal3) / 3;
	//Diffuse Mapping
	vec4 Dif = texture2D( DiffuseTexture, vUV ) * vColor;
	//Pushing to Buffer
	outColor = Dif * NormVal * Light;
	outColor.a = 1.0f;
}