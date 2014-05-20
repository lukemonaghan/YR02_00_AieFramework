#version 330

//Ins
in vec4 VS_Position;
in vec4 VS_Normal;
in vec4 VS_Colour;
in vec2 VS_TexCoord1;

//out final colour
out vec4 outColour;

//camera matrix[3].xyz
uniform vec3 cameraPosition;

//Basic Textures
uniform sampler2D DiffuseTexture;
uniform sampler2D SpecularTexture;
uniform sampler2D NormalTexture;

//Ambient Light
uniform vec3 LightAmbient = vec3(0.1);

//Specular Power(higher = more condensed)
const int SpecPower = 128;
uniform vec3 LightSpecular = vec3(1);

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

//Directional light, takes above, index is simply for the array
vec4 DirectionalLight(int iIndex){
	vec3 v3Normal = normalize( VS_Normal.xyz );
	vec3 v3LightVector = normalize( DirectionalLightDirection[iIndex] );

	vec3 v3Reflect = reflect( v3Normal,-v3LightVector );
	vec3 v3Eye = normalize( cameraPosition - VS_Position.xyz );

	// diffuse
	float fDifDot = max( 0, dot( v3Normal, v3LightVector ) );
	vec3 diffuse = DirectionalLightColour[iIndex].rgb * fDifDot;

	// specular
	float fSpecPowMaxDot = pow( max( 0, dot( v3Eye, v3Reflect ) ), SpecPower );
	vec3 specular = LightSpecular * fSpecPowMaxDot;

	return vec4( diffuse + specular, 1 );
}

//Point light, takes above, index is simply for the array
vec4 PointLight(int iIndex){
	vec3 v3Normal = normalize( VS_Normal.xyz );
	vec3 v3LightVector = normalize(PointLightPosition[iIndex] - VS_Position.xyz);

	vec3 v3Reflect = reflect( -v3LightVector, v3Normal );
	vec3 v3Eye = normalize( cameraPosition - VS_Position.xyz );

	float fDifDot = max( 0, dot( v3Normal, v3LightVector ) );
	vec3 diffuse = PointLightColour[iIndex].rgb;

	// specular
	float fSpecPowMaxDot = pow( max( 0, dot( v3Eye, v3Reflect ) ), SpecPower );
	vec3 specular = LightSpecular * fSpecPowMaxDot;

	float falloff = 1.0f - min(length(PointLightPosition[iIndex] - VS_Position.xyz) / PointLightDistance[iIndex],1.0f);

	return vec4( specular + diffuse, 1 ) * fDifDot * falloff;
}

//WIP Spot light, 
vec4 SpotLight(int iIndex){
	//Light Vector
	vec3 v3LightVector = normalize(SpotLightPosition[iIndex] - VS_Position.xyz);	
	//get the angle between the light vector and the spot lights vector
	float angle = dot(normalize(SpotLightDirection[iIndex]),-v3LightVector);
	angle = max(angle,0);
	//check angle, if smaller then aloud, do stuffs!
	if (acos(angle) < radians(SpotLightAngle[iIndex])){
		//Stuff for specular mapping
		vec3 v3Normal = normalize( VS_Normal.xyz );
		vec3 v3Reflect = reflect( -v3LightVector, v3Normal );
		vec3 v3Eye = normalize( cameraPosition - VS_Position.xyz );
		// specular
		float fSpecPowMaxDot = pow( max( 0, dot( v3Eye, v3Reflect ) ), SpecPower );
		vec3 specular = LightSpecular * fSpecPowMaxDot;

		float fDifDot = max( 0, dot( v3Normal, v3LightVector ) );
		// diffuse
		vec3 diffuse = SpotLightColour[iIndex].rgb;

		float falloff = 1.0f - clamp(length(SpotLightPosition[iIndex] - VS_Position.xyz) / SpotLightDistance[iIndex],0.0f,1.0f);
		float falloff2 = 1.0f - clamp(acos(angle) / radians(SpotLightAngle[iIndex]),0.0f,1.0f);
		return vec4( specular + diffuse , 1.0f ) * fDifDot * falloff * falloff2;
	}
	return vec4(0,0,0,0);
}

void main() {
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
	outColour = texture(DiffuseTexture,VS_TexCoord1) * VS_Colour * v4CombineLightCount;
}