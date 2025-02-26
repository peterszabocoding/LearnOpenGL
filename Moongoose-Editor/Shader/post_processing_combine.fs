// Credit:
// https://lettier.github.io/3d-game-shaders-for-beginners/screen-space-reflection.html

#version 450

// ------------------------------------------------------------------
// INPUT VARIABLES --------------------------------------------------
// ------------------------------------------------------------------


// ------------------------------------------------------------------
// OUTPUT VARIABLES -------------------------------------------------
// ------------------------------------------------------------------

out vec4 FragColor;

// ------------------------------------------------------------------
// UNIFORMS ---------------------------------------------------------
// ------------------------------------------------------------------

layout(binding = 0) uniform sampler2D baseTexture;
layout(binding = 1) uniform sampler2D postProcessingTexture;

uniform float multiply = 1.0;

// ------------------------------------------------------------------
// FUNCTIONS --------------------------------------------------------
// ------------------------------------------------------------------

void main()
{
    vec2 texSize = textureSize(baseTexture, 0).xy;
    vec2 texCoord = gl_FragCoord.xy / texSize; // == TexCoords

    vec4 baseColor = texture(baseTexture, texCoord);
    vec4 postProcessingTexture = texture(postProcessingTexture, texCoord);

    FragColor =  vec4(baseColor + (multiply * postProcessingTexture));
}