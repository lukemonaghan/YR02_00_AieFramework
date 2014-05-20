#version 330

in vec3 Normal;
in vec3 position;

out vec4 fragColour;

uniform vec4 materialColour = vec4(0,0,0,1);
uniform vec3 cameraPosition;

uniform float SpecPower = 128.0f;
uniform vec3 LightSpecular = vec3(1.0f);

uniform vec3 DirectionalLightDirection = vec3(-1,-1,0);
uniform vec3 DirectionalLightColour = vec3(1,1,1);

void main() {
	vec3 v3Normal = normalize( Normal );
	vec3 v3LightVector = normalize( DirectionalLightDirection );

	vec3 v3Reflect = reflect( v3Normal,-v3LightVector );
	vec3 v3Eye = normalize( cameraPosition - position );

	// diffuse
	float fDifDot = max( 0, dot( v3Normal, v3LightVector ) );
	vec3 diffuse = DirectionalLightColour.rgb * fDifDot;

	// specular
	float fSpecPowMaxDot = pow( max( 0, dot( v3Eye, v3Reflect ) ), SpecPower );
	vec3 specular = LightSpecular * fSpecPowMaxDot;

	vec4 NormColour = vec4( diffuse + specular, 1 );

	fragColour = materialColour + NormColour;
}