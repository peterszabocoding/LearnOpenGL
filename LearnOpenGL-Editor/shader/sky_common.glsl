// ------------------------------------------------------------------
// DEFINES ----------------------------------------------------------
// ------------------------------------------------------------------

// Units are in megameters.
#define GROUND_RADIOUS_MM 6.360
#define ATMOSPHERE_RADIOUS_MM 6.460


// ------------------------------------------------------------------
// CONSTANTS --------------------------------------------------------
// ------------------------------------------------------------------

// 200M above the ground.
const vec3 viewPos = vec3(0.0, GROUND_RADIOUS_MM + 0.0002, 0.0);

const vec2 tLUTRes = vec2(256.0, 64.0);
const vec2 msLUTRes = vec2(32.0, 32.0);
// Doubled the vertical skyLUT res from the paper, looks way
// better for sunrise.
const vec2 skyLUTRes = vec2(200.0, 200.0);
