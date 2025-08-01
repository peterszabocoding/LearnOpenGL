#version 450

#include <common.glsl>
#include <sky_common.glsl>

// These are per megameter.
const vec3 rayleighScatteringBase = vec3(5.802, 13.558, 33.1);
const float rayleighAbsorptionBase = 0.0;

const float mieScatteringBase = 3.996;
const float mieAbsorptionBase = 4.4;

const vec3 ozoneAbsorptionBase = vec3(0.650, 1.881, .085);

/*
 * Animates the sun movement.
 */
float getSunAltitude(float time)
{
    const float periodSec = 120.0;
    const float halfPeriod = periodSec / 2.0;
    const float sunriseShift = 0.1;
    float cyclePoint = (1.0 - abs((mod(time,periodSec) - halfPeriod) / halfPeriod));
    cyclePoint = (cyclePoint * (1.0 + sunriseShift))-sunriseShift;
    return (0.5 * PI) * cyclePoint;
}

vec3 getSunDir(float time)
{
    float altitude = getSunAltitude(time);
    return normalize(vec3(0.0, sin(altitude), -cos(altitude)));
}

float getMiePhase(float cosTheta) {
    const float g = 0.8;
    const float scale = 3.0 / (8.0 * PI);
    
    float num = (1.0 - g * g) * (1.0 + cosTheta * cosTheta);
    float denom = (2.0 + g * g) * pow((1.0 + g * g - 2.0 * g * cosTheta), 1.5);
    
    return scale * num / denom;
}

float getRayleighPhase(float cosTheta) {
    const float k = 3.0 / (16.0 * PI);
    return k * (1.0 + cosTheta * cosTheta);
}

void getScatteringValues(vec3 pos, 
                         out vec3 rayleighScattering, 
                         out float mieScattering,
                         out vec3 extinction) {
    float altitudeKM = (length(pos) - GROUND_RADIOUS_MM)*1000.0;
    // Note: Paper gets these switched up.
    float rayleighDensity = exp(-altitudeKM / 8.0);
    float mieDensity = exp(-altitudeKM / 1.2);
    
    rayleighScattering = rayleighScatteringBase * rayleighDensity;
    float rayleighAbsorption = rayleighAbsorptionBase *rayleighDensity;
    
    mieScattering = mieScatteringBase * mieDensity;
    float mieAbsorption = mieAbsorptionBase * mieDensity;
    
    vec3 ozoneAbsorption = ozoneAbsorptionBase * max(0.0, 1.0 - abs(altitudeKM - 25.0) / 15.0);
    
    extinction = rayleighScattering + rayleighAbsorption + mieScattering + mieAbsorption + ozoneAbsorption;
}

float safeacos(const float x) {
    return acos(clamp(x, -1.0, 1.0));
}

// From https://gamedev.stackexchange.com/questions/96459/fast-ray-sphere-collision-code.
float rayIntersectSphere(vec3 ro, vec3 rd, float rad) {
    float b = dot(ro, rd);
    float c = dot(ro, ro) - rad*rad;
    if (c > 0.0f && b > 0.0) return -1.0;
    float discr = b * b - c;
    if (discr < 0.0) return -1.0;
    // Special case: inside sphere, use far discriminant
    if (discr > b * b) return (-b + sqrt(discr));
    return -b - sqrt(discr);
}

/*
 * Same parameterization here.
 */
vec3 getValFromTLUT(sampler2D tex, vec2 bufferRes, vec3 pos, vec3 sunDir) {
    float height = length(pos);
    vec3 up = pos / height;
	float sunCosZenithAngle = dot(sunDir, up);
    vec2 uv = vec2(tLUTRes.x * clamp(0.5 + 0.5 * sunCosZenithAngle, 0.0, 1.0),
                   tLUTRes.y * max(0.0, min(1.0, (height - GROUND_RADIOUS_MM) / (ATMOSPHERE_RADIOUS_MM - GROUND_RADIOUS_MM))));
    uv /= bufferRes;
    return texture(tex, uv).rgb;
}
vec3 getValFromMultiScattLUT(sampler2D tex, vec2 bufferRes, vec3 pos, vec3 sunDir) {
    float height = length(pos);
    vec3 up = pos / height;
	float sunCosZenithAngle = dot(sunDir, up);
    vec2 uv = vec2(msLUTRes.x * clamp(0.5 + 0.5 * sunCosZenithAngle, 0.0, 1.0),
                   msLUTRes.y * max(0.0, min(1.0, (height - GROUND_RADIOUS_MM) / (ATMOSPHERE_RADIOUS_MM - GROUND_RADIOUS_MM))));
    uv /= bufferRes;
    return texture(tex, uv).rgb;
}


layout(binding = 0) uniform sampler2D TransmittanceLUT;
layout(binding = 1) uniform sampler2D MultiscatteringLUT;

// Buffer C calculates the actual sky-view! It's a lat-long map (or maybe altitude-azimuth is the better term),
// but the latitude/altitude is non-linear to get more resolution near the horizon.
const int numScatteringSteps = 32;
vec3 raymarchScattering(vec3 pos, 
                              vec3 rayDir, 
                              vec3 sunDir,
                              float tMax,
                              float numSteps) {
    float cosTheta = dot(rayDir, sunDir);
    
	float miePhaseValue = getMiePhase(cosTheta);
	float rayleighPhaseValue = getRayleighPhase(-cosTheta);
    
    vec3 lum = vec3(0.0);
    vec3 transmittance = vec3(1.0);
    float t = 0.0;
    for (float i = 0.0; i < numSteps; i += 1.0) {
        float newT = ((i + 0.3) / numSteps) * tMax;
        float dt = newT - t;
        t = newT;
        
        vec3 newPos = pos + t * rayDir;
        
        vec3 rayleighScattering, extinction;
        float mieScattering;
        getScatteringValues(newPos, rayleighScattering, mieScattering, extinction);
        
        vec3 sampleTransmittance = exp(-dt * extinction);

        vec3 sunTransmittance = getValFromTLUT(TransmittanceLUT, tLUTRes.xy, newPos, sunDir);
        vec3 psiMS = getValFromMultiScattLUT(MultiscatteringLUT, msLUTRes.xy, newPos, sunDir);
        
        vec3 rayleighInScattering = rayleighScattering * (rayleighPhaseValue * sunTransmittance + psiMS);
        vec3 mieInScattering = mieScattering * (miePhaseValue * sunTransmittance + psiMS);
        vec3 inScattering = (rayleighInScattering + mieInScattering);

        // Integrated scattering within path segment.
        vec3 scatteringIntegral = (inScattering - inScattering * sampleTransmittance) / extinction;

        lum += scatteringIntegral*transmittance;
        
        transmittance *= sampleTransmittance;
    }
    return lum;
}

in vec3 FragPos;
in vec3 EyePosition;
in vec2 TexCoords;
uniform float u_Time;
uniform vec2 resolution;

layout (location = 0) out vec4 bRaymarching;

void main()
{
    if (TexCoords.x * resolution.x >= (skyLUTRes.x + 1.5) || TexCoords.y * resolution.y >= (skyLUTRes.y + 1.5)) {
        bRaymarching = vec4(0.0, 0.0, 0.0, 1.0);
        return;
    }

    float u = clamp(TexCoords.x * resolution.x, 0.0, skyLUTRes.x - 1.0) / skyLUTRes.x;
    float v = clamp(TexCoords.y * resolution.y, 0.0, skyLUTRes.y - 1.0) / skyLUTRes.y;
    
    float azimuthAngle = (u - 0.5) * 2.0 * PI;
    // Non-linear mapping of altitude. See Section 5.3 of the paper.
    float adjV;
    if (v < 0.5) {
		float coord = 1.0 - 2.0 * v;
		adjV = -coord * coord;
	} else {
		float coord = v * 2.0 - 1.0;
		adjV = coord * coord;
	}
    
    float height = length(viewPos);
    vec3 up = viewPos / height;
    float horizonAngle = safeacos(sqrt(height * height - GROUND_RADIOUS_MM * GROUND_RADIOUS_MM) / height) - 0.5 * PI;
    float altitudeAngle = adjV*0.5*PI - horizonAngle;
    
    float cosAltitude = cos(altitudeAngle);
    vec3 rayDir = vec3(cosAltitude * sin(azimuthAngle), sin(altitudeAngle), -cosAltitude * cos(azimuthAngle));
    
    float sunAltitude = (0.5 * PI) - acos(dot(getSunDir(u_Time), up));
    vec3 sunDir = vec3(0.0, sin(sunAltitude), -cos(sunAltitude));
    
    float atmoDist = rayIntersectSphere(viewPos, rayDir, ATMOSPHERE_RADIOUS_MM);
    float groundDist = rayIntersectSphere(viewPos, rayDir, GROUND_RADIOUS_MM);
    float tMax = (groundDist < 0.0) ? atmoDist : groundDist;
    vec3 lum = raymarchScattering(viewPos, rayDir, sunDir, tMax, float(numScatteringSteps));
    bRaymarching = vec4(lum, 1.0);
}