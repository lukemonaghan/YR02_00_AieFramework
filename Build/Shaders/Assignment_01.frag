#version 150

//Ins
in vec3 position;
in vec3 normal;
in vec4 colour;
in vec2 uv;
in vec2 uv2;

//out final colour
out vec4 OutColour;

//camera matrix[3]
uniform vec3 cameraPosition = vec3(1);

//Basic Textures
uniform sampler2D DiffuseTexture;
uniform sampler2D DiffuseTexture2;
uniform sampler2D DiffuseTexture3;
uniform sampler2D SpecularTexture;
uniform sampler2D NormalTexture;

//Ambient Light
uniform vec3 LightAmbient = vec3(0);

//Specular Power(higher = more condensed)
const int SpecPower = 128;
uniform vec3 LightSpecular = vec3(0.1f);

//max of each light type
const int LightMax = 10;

//Directional light information
uniform vec3 DirectionalLightDirection[LightMax];
uniform vec4 DirectionalLightColour[LightMax];
uniform int DirectionalLightCount = 0;

//Point Light information
uniform vec3 PointLightPosition[LightMax];
uniform float PointLightDistance[LightMax];
uniform vec4 PointLightColour[LightMax];
uniform int PointLightCount = 0;

//Point Light information
uniform vec3 SpotLightPosition[LightMax];	
uniform vec3 SpotLightDirection[LightMax];
uniform float SpotLightAngle[LightMax];
uniform float SpotLightDistance[LightMax];
uniform vec4 SpotLightColour[LightMax];
uniform int SpotLightCount = 0;

uniform int ItemID = 0;
vec3 v3Normal;
vec3 v3Spec;

//Directional light
vec4 DirectionalLight(int iIndex){
	vec3 v3LightVector = normalize( DirectionalLightDirection[iIndex] );

	vec3 v3Reflect = reflect( v3Normal, -v3LightVector );
	vec3 v3Eye = normalize( cameraPosition - position  );

	// diffuse
	float fDifDot = max(0,dot( v3Normal, v3LightVector ));
	vec3 diffuse = DirectionalLightColour[iIndex].rgb * fDifDot;

	// specular
	float fSpecPowMaxDot = pow( max(0,dot( v3Eye,v3Reflect) ), SpecPower );
	vec3 specular = v3Spec * fSpecPowMaxDot;

	return vec4( diffuse + specular, 1 );
}

//Point light
vec4 PointLight(int iIndex){
	vec3 v3LightVector = normalize(PointLightPosition[iIndex] - position);

	vec3 v3Reflect = reflect( v3Normal,-v3LightVector );
	vec3 v3Eye = normalize( cameraPosition - position );

	float fDifDot = max(0,dot( v3Normal, v3LightVector ));
	vec3 diffuse = PointLightColour[iIndex].rgb * fDifDot;

	//specular
	float fSpecPowMaxDot = pow( max(0,dot( v3Eye, v3Reflect ) ), SpecPower );
	vec3 specular = v3Spec * fSpecPowMaxDot;

	float falloff = 1.0f - clamp(length(PointLightPosition[iIndex] - position) / PointLightDistance[iIndex],0.0f,1.0f);

	return vec4( diffuse + specular, 1 ) * falloff;
}

//WIP Spot light
vec4 SpotLight(int iIndex){
	//Light Vector
	vec3 v3LightVector = normalize(SpotLightPosition[iIndex] - position);	
	//get the angle between the light vector and the spot lights vector
	float angle = dot(normalize(SpotLightDirection[iIndex]),-v3LightVector);
	angle = max(angle,0);
	//check angle, if smaller then aloud, do stuffs!
	if (acos(angle) < radians(SpotLightAngle[iIndex])){
		//Stuff for specular mapping
		vec3 v3Reflect = reflect( -v3LightVector, v3Normal );
		vec3 v3Eye = normalize( cameraPosition - position );
		// specular
		float fSpecPowMaxDot = pow( max(0, dot( v3Eye, v3Reflect ) ), SpecPower );
		vec3 specular = v3Spec * fSpecPowMaxDot;

		float fDifDot = max(0, dot( v3Normal, v3LightVector ) );
		// diffuse
		vec3 diffuse = SpotLightColour[iIndex].rgb;

		float falloff = 1.0f - clamp(length(SpotLightPosition[iIndex] - position) / SpotLightDistance[iIndex],0.0f,1.0f);
		float falloff2 = 1.0f - clamp(acos(angle) / radians(SpotLightAngle[iIndex]),0.0f,1.0f);
		return vec4( diffuse + specular , 1.0f ) * fDifDot * falloff * falloff2;
	}
	return vec4(0,0,0,0);
}

void main() {
	if (ItemID == 1){
		//generate the heightmaped floor
		vec4 Grass = texture(DiffuseTexture,uv);
		vec4 Dirt = texture(DiffuseTexture2,uv);
		vec4 Path = texture(DiffuseTexture3,uv2);
		OutColour = mix(Dirt,Grass,Path.r);
		v3Normal = normalize( vec3(0,1,0) );
		v3Spec = vec3(0.1f);
	}else{
		v3Normal = normalize( normal );
		OutColour = texture(DiffuseTexture,uv);
		v3Spec = LightSpecular;
	}
	//Create the combined float value and add each lights return value to the previous, starts off as the ambient light
	vec4 v4CombineLightCount = vec4(LightAmbient,1);
	//Directional
	for (int i = 0; i < DirectionalLightCount; i++){
		v4CombineLightCount += DirectionalLight(i);
	}
	//Point
	for (int i = 0; i < PointLightCount; i++){
		v4CombineLightCount += PointLight(i);
	}
	//Spot
	for (int i = 0; i < SpotLightCount; i++){
		v4CombineLightCount += SpotLight(i);
	}
	//Generic outcolour
	OutColour *= colour * v4CombineLightCount;
	if (OutColour.a < 0.1f){discard;}
}