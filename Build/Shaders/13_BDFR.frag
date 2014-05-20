#version 330

in vec2 vUV;
in vec4 vColor;
in vec4 vNormal;

out vec4 outColor;

uniform vec3 Camera;

uniform vec3 LightDir = vec3(0,0,-1);
uniform vec3 LightColour = vec3(0,1,0);

uniform sampler2D DiffuseTexture;
uniform sampler2D SpecularTexture;
uniform sampler2D NormalTexture;

uniform float roughness = 0.0f;
uniform float FresnelScale = 1.0f;

float OrenNayar(){
	vec3 N = texture( NormalTexture, vUV ).xyz * 2 - 1;
	vec3 L = normalize(LightDir);
	vec3 E = Camera;

	float NdL = max( 0.0f, dot( N, L ) ); 
	float NdE = max( 0.0f, dot( N, E ) ); 
	 
	float R2 = roughness * roughness; 
	 
	// Oren-Nayar Diffuse Term 
	float A = 1.0f - 0.5f * R2 / (R2 + 0.33f); 
	float B = 0.45f * R2 / (R2 + 0.09f); 
	 
	// CX = Max(0, cos(l,e)) 
	vec3 lightProjected = normalize( L - N * NdL ); 
	vec3 viewProjected = normalize( E - N * NdE); 
	float CX = max( 0.0f, dot( lightProjected, viewProjected ) ); 
	 
	// DX = sin(alpha) * tan(beta) 
	float alpha = sin( max( acos( NdE ), acos( NdL ) ) ); 
	float beta = tan( min( acos( NdE ), acos( NdL ) ) ); 
	float DX = alpha * beta; 
	 
	// Calculate Oren-Nayar, replaces the Phong Lambertian Term 
	return (NdL * (A + B * CX * DX)); 
}

float CookTorrance(){
	vec3 N = texture( NormalTexture, vUV ).xyz * 2 - 1;
	vec3  L   = normalize(LightDir);
	vec3  E   = Camera;

	float NdL = max( 0.0f, dot( N, L ) ); 
	float NdE = max( 0.0f, dot( N, E ) ); 

	float R2  = roughness * roughness; 
	vec3  H   = normalize( L + E );
	float HdE = dot( H, E ); 

	float NdH = max( 0.0f, dot( N, H ) ); 
	float NdH2 = NdH * NdH; 
	float e   = 2.71828182845904523536028747135f; 
	float pi  = 3.1415926535897932384626433832f; 
	 
	// Beckman's Distribution Function D 
	float exponent = -(1 - NdH2) / (NdH2 * R2); 
	float D = pow( e, exponent ) / (R2 * NdH2 * NdH2); 
	 
	// Fresnel Term F 
	float F = mix( pow( 1 - HdE, 5 ), 1, FresnelScale ); 
	 
	// Geometric Attenuation Factor G 
	float X = 2.0f * NdH / dot( E, H ); 
	float G = min(1, min(X * NdL, X * NdE)); 
	 
	// Calculate Cook-Torrance 
	return max( (D*G*F) / (NdE * pi), 0.0f ); 
}

void main() { 
	outColor = texture2D( DiffuseTexture, vUV.xy ) * (OrenNayar() + CookTorrance());
	outColor.a = 1.0f;
}