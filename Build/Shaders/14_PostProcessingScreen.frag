#version 330

in vec2 textureCoordinate;

out vec4 outColour;

uniform sampler2D diffuseMap;
uniform sampler2D depthMap;

uniform int filterMode = 0;
uniform int DrawMode = 0;

uniform float time = 0;
uniform int width = 1280;
uniform int height = 720;

uniform float Near = 0.1f;
uniform float Far = 50.0f;

uniform float FogThickness = 1.0f;

float LinearDepth(float DepthBufferZ){
	return (2.0 * Near) / (Far + Near - DepthBufferZ * (Far - Near));
}

bool CheckRange(vec4 C1, vec4 C2,float range){
	if ((C1.r > C2.r + range || C1.r < C2.r - range) || 
	    (C1.g > C2.g + range || C1.g < C2.g - range) || 
	    (C1.b > C2.b + range || C1.b < C2.b - range) || 
	    (C1.a > C2.a + range || C1.a < C2.a - range)){
		return true;
	}
	return false;
}

// simple box blur fragment shader function
vec4 simpleBlur(vec4 InColour) {
	vec2 blurRange = 1.0f / textureSize( diffuseMap, 0 ).xy;
	vec4 BlurColour = InColour;
	BlurColour += texture(diffuseMap,textureCoordinate + vec2(-blurRange.x,blurRange.y));
	BlurColour += texture(diffuseMap,textureCoordinate + vec2(-blurRange.x,0));
	BlurColour += texture(diffuseMap,textureCoordinate + vec2(-blurRange.x,-blurRange.y));
	BlurColour += texture(diffuseMap,textureCoordinate + vec2(0,blurRange.y));
	BlurColour += texture(diffuseMap,textureCoordinate + vec2(0,-blurRange.y));
	BlurColour += texture(diffuseMap,textureCoordinate + vec2(blurRange.x,blurRange.y));
	BlurColour += texture(diffuseMap,textureCoordinate + vec2(blurRange.x,0));
	BlurColour += texture(diffuseMap,textureCoordinate + vec2(blurRange.x,-blurRange.y));

	return BlurColour / 9;
}
// distort fragment shader function
void distort(float Weight) {
	float distanceFromCentre = distance(textureCoordinate,vec2(0.5,0.5));
	vec2 normalizedCoord = normalize(textureCoordinate - vec2(0.5,0.5));
	float bias = distanceFromCentre + (sin(distanceFromCentre * 15)*Weight);
	vec2 newCoordinate = vec2(0.5,0.5) + (bias * normalizedCoord);
	outColour = texture( diffuseMap, newCoordinate );
}
//RimLighting
void OutLine(vec3 OutlineColour,float Range){
	vec2 blurRange = 1.0f / vec2(width,height);
	if ( 
		CheckRange ( texture(diffuseMap,textureCoordinate + vec2(-blurRange.x,blurRange.y))	,outColour,Range) || 
		CheckRange ( texture(diffuseMap,textureCoordinate + vec2(-blurRange.x,0)) 			,outColour,Range) || 
		CheckRange ( texture(diffuseMap,textureCoordinate + vec2(-blurRange.x,-blurRange.y)),outColour,Range) || 
		CheckRange ( texture(diffuseMap,textureCoordinate + vec2(0,blurRange.y)) 			,outColour,Range) || 
		CheckRange ( texture(diffuseMap,textureCoordinate + vec2(0,-blurRange.y)) 			,outColour,Range) || 
		CheckRange ( texture(diffuseMap,textureCoordinate + vec2(blurRange.x,blurRange.y)) 	,outColour,Range) || 
		CheckRange ( texture(diffuseMap,textureCoordinate + vec2(blurRange.x,0)) 			,outColour,Range) || 
		CheckRange ( texture(diffuseMap,textureCoordinate + vec2(blurRange.x,-blurRange.y)) ,outColour,Range) 
		){
		outColour = vec4(OutlineColour,1);
	}
}
//ViewFog
void DistanceColour(float strength){
	float distanceFromCentre = distance(textureCoordinate,vec2(0.5,0.5));
	vec2 normalizedCoord = normalize(textureCoordinate - vec2(0.5,0.5));
	float bias = 1.0f - distanceFromCentre + (sin(distanceFromCentre * 15)*strength);
	outColour *= bias;
}

void ScanLinesY(int spacing,int movement){
	float yPos = int((1.0f + (textureCoordinate.y) ) * height / 2) + int(time*movement);
	if (mod(yPos,2.0f * spacing) == 1.0f){
		outColour *= vec4(0.3);
	}
}

void ScanLinesX(int spacing,int movement){
	float xPos = int((1.0f + (textureCoordinate.x) ) * width / 2) + int(time*movement);
	if (mod(xPos,2.0f * spacing) == 1.0f){
		outColour *= vec4(0.3);
	}
}

void Invert() {
	outColour.rgb = 1.0f - outColour.rgb;
}

void FlipY(){
	vec2 InvertedY = vec2(textureCoordinate.x,1.0f - textureCoordinate.y); 
	outColour = texture( diffuseMap, InvertedY );
}

void FlipX(){
	vec2 InvertedY = vec2(1.0f - textureCoordinate.x,textureCoordinate.y); 
	outColour = texture( diffuseMap, InvertedY );
}

float calculateBlurStrength(float focusNear,float focus,float focusFar) { 
	float f; 
	float depth = LinearDepth(texture( depthMap, textureCoordinate ).r);
	if ( depth < focus ) {
		f = (depth - focus) / (focus - focusNear); 
	} else {
		f = (depth - focus) / (focusFar - focus); 
	} return min( 1, abs( f ) ); 
}

void DOF(){
	vec2 blurRange = 1.0f / vec2(width,height);
	float CenterDepth = LinearDepth(texture( depthMap, vec2(0.5f) ).r);
	float BlurStrength = calculateBlurStrength(0.0f,CenterDepth,0.0f);
	vec4 BlurColour = simpleBlur(outColour);
	outColour = mix(outColour,BlurColour,BlurStrength);
}

void Fog(){
	float d = LinearDepth(texture( depthMap, textureCoordinate ).r);
	outColour += d * FogThickness;
}

void Sobel(){
	vec2 blurRange = 1.0f / vec2(width,height);
	vec4 center = texture2D(diffuseMap, textureCoordinate);
	vec4 left   = texture2D(diffuseMap, textureCoordinate - vec2(blurRange.x, 0.0));
	vec4 right  = texture2D(diffuseMap, textureCoordinate + vec2(blurRange.x, 0.0));
	vec4 up     = texture2D(diffuseMap, textureCoordinate - vec2(0.0, blurRange.y));
	vec4 down   = texture2D(diffuseMap, textureCoordinate + vec2(0.0, blurRange.y));
	vec4 leftDiff  = center - left;
	vec4 rightDiff = center - right;
	vec4 upDiff    = center - up;
	vec4 downDiff  = center - down;
	vec4 total = clamp(leftDiff + rightDiff + upDiff + downDiff, 0.0, 1.0);
	outColour = vec4(total.rgb, center.a);
}

void Pencil(){
	vec2 OneTexel = 1.0f / vec2(width,height);
	float LumaLevel = 64.0f;
	float LumaRamp = 16.0f;
	vec4 center = texture2D(diffuseMap, textureCoordinate);
    vec4 up     = texture2D(diffuseMap, textureCoordinate + vec2(        0.0, -OneTexel.y));
    vec4 up2    = texture2D(diffuseMap, textureCoordinate + vec2(        0.0, -OneTexel.y) * 2.0);
    vec4 down   = texture2D(diffuseMap, textureCoordinate + vec2( OneTexel.x,         0.0));
    vec4 down2  = texture2D(diffuseMap, textureCoordinate + vec2( OneTexel.x,         0.0) * 2.0);
    vec4 left   = texture2D(diffuseMap, textureCoordinate + vec2(-OneTexel.x,         0.0));
    vec4 left2  = texture2D(diffuseMap, textureCoordinate + vec2(-OneTexel.x,         0.0) * 2.0);
    vec4 right  = texture2D(diffuseMap, textureCoordinate + vec2(        0.0,  OneTexel.y));
    vec4 right2 = texture2D(diffuseMap, textureCoordinate + vec2(        0.0,  OneTexel.y) * 2.0);
    vec4 uDiff = abs(center - up);
    vec4 dDiff = abs(center - down);
    vec4 lDiff = abs(center - left);
    vec4 rDiff = abs(center - right);
    vec4 u2Diff = abs(center - up2);
    vec4 d2Diff = abs(center - down2);
    vec4 l2Diff = abs(center - left2);
    vec4 r2Diff = abs(center - right2);
    vec4 sum = uDiff + dDiff + lDiff + rDiff + u2Diff + d2Diff + l2Diff + r2Diff;
    vec4 gray = vec4(0.3, 0.59, 0.11, 0.0);
    float sumLuma = 1.0 - dot(clamp(sum, 0.0, 1.0), gray);
    
    // Get luminance of center pixel and adjust
    float centerLuma = dot(center + (center - pow(center, vec4(LumaRamp))), gray);
    
    // Quantize the luma value
    centerLuma = centerLuma - fract(centerLuma * LumaLevel) / LumaLevel;
    
    // Re-scale to full range
    centerLuma = centerLuma * (LumaLevel / (LumaLevel - 1.0));
    
    // Blend with outline
    centerLuma = centerLuma * sumLuma;
    
    outColour = vec4(centerLuma, centerLuma, centerLuma, center.a);
}

void Blob(){
	vec2 oneTexel = 1.0f / vec2(width,height);

	vec4 u  = texture2D(diffuseMap, textureCoordinate + vec2(        0.0, -oneTexel.y));
	vec4 d  = texture2D(diffuseMap, textureCoordinate + vec2(        0.0,  oneTexel.y));
	vec4 l  = texture2D(diffuseMap, textureCoordinate + vec2(-oneTexel.x,         0.0));
	vec4 r  = texture2D(diffuseMap, textureCoordinate + vec2( oneTexel.x,         0.0));
	vec4 ul = texture2D(diffuseMap, textureCoordinate + vec2(-oneTexel.x, -oneTexel.y));
	vec4 dr = texture2D(diffuseMap, textureCoordinate + vec2( oneTexel.x,  oneTexel.y));
	vec4 dl = texture2D(diffuseMap, textureCoordinate + vec2(-oneTexel.x,  oneTexel.y));
	vec4 ur = texture2D(diffuseMap, textureCoordinate + vec2( oneTexel.x, -oneTexel.y));
	vec4 uu = texture2D(diffuseMap, textureCoordinate + vec2(              0.0, -oneTexel.y * 2.0));
	vec4 dd = texture2D(diffuseMap, textureCoordinate + vec2(              0.0,  oneTexel.y * 2.0));
	vec4 ll = texture2D(diffuseMap, textureCoordinate + vec2(-oneTexel.x * 2.0,               0.0));
	vec4 rr = texture2D(diffuseMap, textureCoordinate + vec2( oneTexel.x * 2.0,               0.0));

	vec4 v1 = min(l, r);
	vec4 v2 = min(u, d);
	vec4 v4 = min(ul, dr);
	vec4 v5 = min(ur, dl);
	vec4 v8 = min(uu, dd);
	vec4 v9 = min(ll, rr);

	vec4 v3 = min(v1, v2);
	vec4 v6 = min(v4, v5);
	vec4 v10 = min(v8, v9);
	vec4 v7 = min(v3, v6);
	vec4 v11 = min(v7, v10);

	vec4 c  = texture2D(diffuseMap, textureCoordinate);
	vec4 color = min(c, v11);
	outColour = vec4(color.rgb, c.a);
}

void Sharpen(int strength){
	vec2 oneTexel = 1.0f / vec2(width,height);
	vec4 c = texture2D(diffuseMap, textureCoordinate);
    vec4 u = texture2D(diffuseMap, textureCoordinate + vec2(        0.0, -oneTexel.y));
    vec4 d = texture2D(diffuseMap, textureCoordinate + vec2(        0.0,  oneTexel.y));
    vec4 l = texture2D(diffuseMap, textureCoordinate + vec2(-oneTexel.x,         0.0));
    vec4 r = texture2D(diffuseMap, textureCoordinate + vec2( oneTexel.x,         0.0));

    vec4 nc = normalize(c);
    vec4 nu = normalize(u);
    vec4 nd = normalize(d);
    vec4 nl = normalize(l);
    vec4 nr = normalize(r);

    float du = dot(nc, nu);
    float dd = dot(nc, nd);
    float dl = dot(nc, nl);
    float dr = dot(nc, nr);

    float f = 1.0;
    f += (du * strength) - (dd * strength);
    f += (dr * strength) - (dl * strength);

    vec4 color = c * clamp(f, 0.5, 2);
    outColour = vec4(color.rgb, c.a);
}

void Blobs2(float cperchannel){
	outColour = ceil(texture( diffuseMap, textureCoordinate )*cperchannel)/cperchannel;
}

void BlackWhite(){
	outColour.rgb = vec3((outColour.r+outColour.g+outColour.b)/3.0f);
}

void main() {
	if (DrawMode == 0){
		outColour = texture( diffuseMap, textureCoordinate );
	}else{
		outColour = vec4(LinearDepth(texture( depthMap, textureCoordinate ).r));
		return;
	}
	if (filterMode == 1){
		outColour = simpleBlur(outColour);
	}if (filterMode == 2){
		distort(0.05f);
	}if (filterMode == 3){
		OutLine(vec3(0),0.15f);
	}if (filterMode == 4){
		DistanceColour(0.005f);
	}if (filterMode == 5){
		ScanLinesY(1,0);
	}if (filterMode == 6){
		ScanLinesX(1,0);
	}if (filterMode == 7){
		Invert();
	}if (filterMode == 8){
		FlipY();
	}if (filterMode == 9){
		FlipX();
	}if (filterMode == 10){
		DOF();
	}if (filterMode == 11){
		DOF();
		Fog();
		ScanLinesY(2,0);
		outColour.r *= 0.1f;
		outColour.b *= 0.1f;
		DistanceColour(0.05f);
	}if (filterMode == 12){
		Sobel();
	}if (filterMode == 13){
		Pencil();
	}if (filterMode == 14){
		Blob();
	}if (filterMode == 15){
		Sharpen(32);
	}if (filterMode == 16){
		Blobs2(4);
	}if (filterMode == 17){
		BlackWhite();
	}
}