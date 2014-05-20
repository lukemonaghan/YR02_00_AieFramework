#version 330

in vec3 VS_Normal; 
in vec3 VS_LightDir;
in vec2 VS_TexCoord1;

uniform sampler2D DiffuseTexture;
uniform sampler2D SpecularTexture;
uniform sampler2D NormalTexture;

uniform vec3 AmbientLightColor = vec3(0.2);
uniform vec3 LightColor = vec3(1,0,0);
uniform float Time;

out vec4 outColor;

void main() {

	vec3 LightDir = VS_LightDir;
	LightDir.x = sin(Time) * 2.5f;
	LightDir.z = cos(Time) * 2.5f;
	
	// get the color from our textures
	vec4 diffuseColor	= texture( DiffuseTexture, VS_TexCoord1);
	vec4 specularColor	= texture( SpecularTexture,VS_TexCoord1);
	vec4 normalColor	= texture( NormalTexture, VS_TexCoord1);
	
	// calculate lighting with dot product
	vec3 N = normalize((2.0 * normalColor.rgb - 1.0) + VS_Normal);
	vec3 L = normalize(LightDir);
	
	// how bright should this pixel be based on our light and view direction
	float di = max(0.0, dot(N, L));
	vec4 diffuseIntensity = vec4(di,di,di,1);

	// for simplicity, will use the diffuse texture as the ambient color
	vec4 ambientColor = diffuseColor * vec4(AmbientLightColor, 1.0);	

	// specular hilight
	vec3 reflection = normalize( reflect( -L, N));
	float spec = max(0.0, dot( reflection, N));
	float fSpec = pow(spec, 5.0);
	vec4 SpecularHighlight = vec4(fSpec,fSpec,fSpec,1);

	//setup and push to buffer
	outColor = (diffuseColor * diffuseIntensity ) + (specularColor * SpecularHighlight) + ambientColor;	
}