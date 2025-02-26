// Credit:
// https://lettier.github.io/3d-game-shaders-for-beginners/screen-space-reflection.html

#version 450

// ------------------------------------------------------------------
// INPUT VARIABLES --------------------------------------------------
// ------------------------------------------------------------------

in vec2 TexCoords;

// ------------------------------------------------------------------
// OUTPUT VARIABLES -------------------------------------------------
// ------------------------------------------------------------------

out vec4 FragColor;

// ------------------------------------------------------------------
// UNIFORMS ---------------------------------------------------------
// ------------------------------------------------------------------

layout(binding = 0) uniform sampler2D renderTexture;
layout(binding = 1) uniform sampler2D gPositionTexture;
layout(binding = 2) uniform sampler2D gNormalTexture;
layout(binding = 3) uniform sampler2D gRoughnessTexture;
layout(binding = 4) uniform sampler2D gDepthTexture;

uniform mat4 projection;
uniform mat4 view;

uniform float maxDistance;
uniform float resolution;
uniform float thickness;
uniform int steps;

// ------------------------------------------------------------------
// FUNCTIONS --------------------------------------------------------
// ------------------------------------------------------------------

float map(float value, float min1, float max1, float min2, float max2) {
  return min2 + (value - min1) * (max2 - min2) / (max1 - min1);
}

// ------------------------------------------------------------------

vec4 viewToUV(vec4 point, mat4 projection, vec2 textureSize)
{
    // Project to screen space.
    point = projection * point;
     // Perform the perspective divide.
    point.xyz /= point.w;
    // Convert the screen-space XY coordinates to UV coordinates.
    point.xy = point.xy * 0.5 + 0.5;
    // Convert the UV coordinates to fragment/pixel coordnates.
    point.xy *= textureSize;

    return point;
}

// ------------------------------------------------------------------

float perspectiveDepthToViewZ( const in float depth, const in float near, const in float far ) {
	// maps perspective depth in [ 0, 1 ] to viewZ
	return ( near * far ) / ( ( far - near ) * depth - far );
}

// ------------------------------------------------------------------

void main()
{
    int localSteps = steps;

    vec2 texSize = textureSize(gPositionTexture, 0).xy;
    vec2 texCoord = gl_FragCoord.xy / texSize; // == TexCoords
    vec4 uv = vec4(0.0);

    vec4 renderPixelColor = texture(renderTexture, texCoord);
    vec4 positionFrom = texture(gPositionTexture, texCoord);
    vec3 unitPositionFrom = normalize(positionFrom.xyz);

    vec3 viewSpaceNormal = normalize(texture(gNormalTexture, texCoord).xyz);
    vec3 pivot = normalize(reflect(unitPositionFrom, viewSpaceNormal));

    vec4 positionTo = positionFrom;
    vec4 startView = vec4(positionFrom.xyz, 1);
    vec4 endView   = vec4(positionFrom.xyz + (pivot * maxDistance), 1);

    vec4 startFrag = gl_FragCoord;
    vec4 endFrag = viewToUV(endView, projection, texSize);

    vec2 frag = startFrag.xy;
    uv.xy = frag / texSize;

    float deltaX = endFrag.x - startFrag.x;
    float deltaY = endFrag.y - startFrag.y;

    float useX = abs(deltaX) >= abs(deltaY) ? 1 : 0;
    float delta = mix(abs(deltaY), abs(deltaX), useX) * clamp(resolution, 0, 1);

    vec2  increment = vec2(deltaX, deltaY) / max(delta, 0.001);
    float search0 = 0;
    float search1 = 0;

    int hit0 = 0;
    int hit1 = 0;

    float viewDistance = -startView.z;
    float depth = thickness;

    float totalStep = min(max(abs(deltaX), abs(deltaY)), 1000);

    for (int i = 0; i < int(delta); ++i) {

        if (i > totalStep) break;

        frag += increment;
        uv.xy = frag / texSize;
        positionTo = texture(gPositionTexture, uv.xy);

        search1 = mix( (frag.y - startFrag.y) / deltaY, (frag.x - startFrag.x) / deltaX, useX);

        // Perspective corrected linear interpolation
        // https://www.comp.nus.edu.sg/~lowkl/publications/lowk_persp_interp_techrep.pdf
        viewDistance = startView.z * endView.z / -mix(endView.z, startView.z, search1);

        depth = viewDistance - abs(positionTo.z);

        if (depth > 0 && depth < thickness) {
            hit0 = 1;
            break;
        } else {
            search0 = search1;
        }
    }

    search1 = search0 + ((search1 - search0) / 2.0);
    localSteps *= hit0;

    for (int j = 0; j < localSteps; ++j) {
        frag = mix(startFrag.xy, endFrag.xy, search1);
        uv.xy = frag / texSize;
        positionTo = texture(gPositionTexture, uv.xy);

        viewDistance = startView.z * endView.z / -mix(endView.z, startView.z, search1);
        depth = viewDistance - abs(positionTo.z);

        if (depth > 0 && depth < thickness) {
            hit1 = 1;
            search1 = search0 + ((search1 - search0) / 2);
        } else {
            float temp = search1;
            search1 = search1 + ((search1 - search0) / 2);
            search0 = temp;
        }
    }

    float visibility = hit1
    * positionTo.w // no scene position available
    * (1 - max( dot(-unitPositionFrom, pivot), 0)) // fade out if reflected vector points towards the camera
    * ( 1 - clamp(depth / thickness, 0, 1)) // distance of reflection from the intersection point
    * ( 1 - clamp(length(positionTo - positionFrom) / maxDistance, 0, 1)) // fade out if reflection goes beyond maxDistance
    * (uv.x < 0 || uv.x > 1 ? 0 : 1) // outside of camera frustum
    * (uv.y < 0 || uv.y > 1 ? 0 : 1);

    visibility = clamp(visibility, 0, 1);
    uv.ba = vec2(visibility);

    float roughness = 1 - texture(gRoughnessTexture, texCoord).r;
    vec3 reflection = mix(vec3(0), texture(renderTexture, uv.xy).rgb, uv.b);

    //vec3 color = mix(renderPixelColor.rgb, mix(renderPixelColor.rgb, reflection, roughness), uv.a);
    vec3 color = mix(vec3(0), mix(vec3(0), reflection, roughness), uv.a);
    FragColor =  vec4(color, 1.0);
}