
#define PI 3.1415926535897932

float saturate(float value) {
	return clamp(value, 0.0, 1.0);
}

uint ReverseBits32(uint bits) {
	bits = (bits << 16) | (bits >> 16);
	bits = ((bits & 0x00ff00ffU) << 8) | ((bits & 0xff00ff00U) >> 8);
	bits = ((bits & 0x0f0f0f0fU) << 4) | ((bits & 0xf0f0f0f0U) >> 4);
	bits = ((bits & 0x33333333U) << 2) | ((bits & 0xccccccccU) >> 2);
	bits = ((bits & 0x55555555U) << 1) | ((bits & 0xaaaaaaaaU) >> 1);

	return bits;
}

vec2 Hammersley(uint Index, uint NumSamples) {
	float E1 = fract(float(Index) / float(NumSamples));
	float E2 = float(ReverseBits32(Index)) * 2.3283064365386963e-10;

	return vec2(E1, E2);
}
/*
vec2 Random(vec3 pixel, float seed) {
	const vec3 scale1 = { 12.9898f, 78.233f, 151.7182f };
	const vec3 scale2 = { 63.7264f, 10.873f, 623.6736f };

	vec2 Xi;

	Xi.x = fract(sin(dot(pixel + seed, scale1)) * 43758.5453 + seed);
	Xi.y = fract(sin(dot(pixel + seed, scale2)) * 43758.5453 + seed);

	return Xi;
}

vec3 CosineSample(vec2 Xi) {
	float phi = 2 * PI * Xi.x;
	float costheta = sqrt(Xi.y);
	float sintheta = sqrt(1 - costheta * costheta);

	return vec3(sintheta * cos(phi), sintheta * sin(phi), costheta);
}
*/
vec3 GGXSample(vec2 Xi, float roughness) {
	float a = roughness * roughness;
	float a2 = a * a;

	float phi = 2 * PI * Xi.x;
	float costheta = sqrt((1 - Xi.y) / (1 + (a2 - 1) * Xi.y));
	float sintheta = sqrt(1 - costheta * costheta);

	return vec3(sintheta * cos(phi), sintheta * sin(phi), costheta);
}

vec3 TangentToWorld(vec3 Vec, vec3 TangentZ) {
	vec3 UpVector = abs(TangentZ.z) < 0.999 ? vec3(0, 0, 1) : vec3(1, 0, 0);
	vec3 TangentX = normalize(cross(UpVector, TangentZ));
	vec3 TangentY = cross(TangentZ, TangentX);

	return TangentX * Vec.x + TangentY * Vec.y + TangentZ * Vec.z;
}
