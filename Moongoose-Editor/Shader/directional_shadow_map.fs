#version 450                                            

// ------------------------------------------------------------------
// OUTPUT VARIABLES -------------------------------------------------
// ------------------------------------------------------------------

out vec4 color;

// ------------------------------------------------------------------

void main()
{
	float ndcDepth = (2.0 * gl_FragCoord.z - gl_DepthRange.near - gl_DepthRange.far) / (gl_DepthRange.far - gl_DepthRange.near);
	float clipDepth = ndcDepth / gl_FragCoord .w;
	color = vec4((clipDepth * 0.5) + 0.5);
} 