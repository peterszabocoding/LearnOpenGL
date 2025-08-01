#version 450

// ------------------------------------------------------------------
// DEFINES ----------------------------------------------------------
// ------------------------------------------------------------------

#define PI 3.14159265358

// Units are in megameters.
#define GROUND_RADIOUS_MM 6.360
#define ATMOSPHERE_RADIOUS_MM 6.460

#define RAYLEIGH_SCATTERING_BASE vec3(5.802, 13.558, 33.1)
#define RAYLEIGH_ABSORPTION_BASE 0.0

#define MIE_SCATTERING_BASE 3.996
#define MIE_ABSORPTION_BASE 4.4

#define OZONE_ABSORPTION_BASE vec3(0.650, 1.881, .085)

// ------------------------------------------------------------------
// CONSTANTS --------------------------------------------------------
// ------------------------------------------------------------------

const vec2 tLUTRes = vec2(256.0, 64.0);

// Buffer A generates the Transmittance LUT. Each pixel coordinate corresponds to a height and sun zenith angle, and
// the value is the transmittance from that point to sun, through the atmosphere.
const float sunTransmittanceSteps = 40.0;

// ------------------------------------------------------------------
// INPUT VARIABLES --------------------------------------------------
// ------------------------------------------------------------------

in vec2 TexCoords;

// ------------------------------------------------------------------
// OUTPUT VARIABLES -------------------------------------------------
// ------------------------------------------------------------------

layout (location = 0) out vec4 bTransmittance;

// ------------------------------------------------------------------
// UNIFORMS ---------------------------------------------------------
// ------------------------------------------------------------------

uniform vec2 resolution;

// ------------------------------------------------------------------
// FUNCTIONS --------------------------------------------------------
// ------------------------------------------------------------------

void getScatteringValues(vec3 pos, 
                         out vec3 rayleighScattering, 
                         out float mieScattering,
                         out vec3 extinction) {
    float altitudeKM = (length(pos)-GROUND_RADIOUS_MM) * 1000.0;

    // Note: Paper gets these switched up.
    float rayleighDensity = exp(-altitudeKM/8.0);
    float mieDensity = exp(-altitudeKM/1.2);
    
    rayleighScattering = RAYLEIGH_SCATTERING_BASE * rayleighDensity;
    float rayleighAbsorption = RAYLEIGH_ABSORPTION_BASE * rayleighDensity;
    
    mieScattering = MIE_SCATTERING_BASE * mieDensity;
    float mieAbsorption = MIE_ABSORPTION_BASE * mieDensity;
    
    vec3 ozoneAbsorption = OZONE_ABSORPTION_BASE * max(0.0, 1.0 - abs(altitudeKM-25.0)/15.0);
    
    extinction = rayleighScattering + rayleighAbsorption + mieScattering + mieAbsorption + ozoneAbsorption;
}

// ------------------------------------------------------------------

float safeacos(const float x) {
    return acos(clamp(x, -1.0, 1.0));
}

// ------------------------------------------------------------------

// From https://gamedev.stackexchange.com/questions/96459/fast-ray-sphere-collision-code.
float rayIntersectSphere(vec3 ro, vec3 rd, float rad) {
    float b = dot(ro, rd);
    float c = dot(ro, ro) - rad*rad;
    if (c > 0.0f && b > 0.0) return -1.0;
    
    float discr = b*b - c;
    if (discr < 0.0) return -1.0;

    // Special case: inside sphere, use far discriminant
    if (discr > b*b) return (-b + sqrt(discr));
    return -b - sqrt(discr);
}

// ------------------------------------------------------------------

vec3 getSunTransmittance(vec3 pos, vec3 sunDir) {
    if (rayIntersectSphere(pos, sunDir, GROUND_RADIOUS_MM) > 0.0) {
        return vec3(0.0);
    }
    
    float atmoDist = rayIntersectSphere(pos, sunDir, ATMOSPHERE_RADIOUS_MM);
    float t = 0.0;
    
    vec3 transmittance = vec3(1.0);
    for (float i = 0.0; i < sunTransmittanceSteps; i += 1.0) {
        float newT = ((i + 0.3) / sunTransmittanceSteps) * atmoDist;
        float dt = newT - t;
        t = newT;
        
        vec3 newPos = pos + t * sunDir;
        
        vec3 rayleighScattering, extinction;
        float mieScattering;
        getScatteringValues(newPos, rayleighScattering, mieScattering, extinction);
        
        transmittance *= exp(-dt*extinction);
    }
    return transmittance;
}

// ------------------------------------------------------------------

void main()
{
    if (TexCoords.x * resolution.x >= (tLUTRes.x + 1.5) || TexCoords.y * resolution.y >= (tLUTRes.y + 1.5)) {
        bTransmittance = vec4(0.0, 0.0, 0.0, 1.0);
        return;
    }

    float u = clamp(TexCoords.x * resolution.x, 0.0, resolution.x - 1.0) / resolution.x;
    float v = clamp(TexCoords.y * resolution.y, 0.0, resolution.y - 1.0) / resolution.y;
    
    float sunCosTheta = 2.0 * u - 1.0;
    float sunTheta = safeacos(sunCosTheta);
    float height = mix(GROUND_RADIOUS_MM, ATMOSPHERE_RADIOUS_MM, v);
    
    vec3 pos = vec3(0.0, height, 0.0); 
    vec3 sunDir = normalize(vec3(0.0, sunCosTheta, -sin(sunTheta)));
    
    bTransmittance = vec4(getSunTransmittance(pos, sunDir), 1.0);
}
