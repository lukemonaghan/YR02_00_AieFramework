
void main() {
	vec4 Diffuse = texture2D(DiffuseTexture,VS_TexCoord1);
	vec4 Normal = texture2D(NormalTexture,VS_TexCoord1);
	vec4 Position = texture2D(WorldTexture,VS_TexCoord1);

	vec3 Light = vec3(0,5,0);
	vec3 LightDir = Light - Position.xyz;

	Normal = normalize(Normal);
	LightDir = normalize(LightDir);

	vec3 EyeDir = normalize(CameraPosition - Position.xyz);
	vec3 HalfVector = normalize(LightDir.xyz+EyeDir);

	outColour = Diffuse * max(dot(Normal.xyz,LightDir),0) + pow(max(dot(Normal.xyz,HalfVector),0.0),128) * 1.5f;
}