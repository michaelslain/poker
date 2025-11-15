#version 410

in vec2 fragTexCoord;
in vec4 fragColor;

out vec4 finalColor;

uniform vec4 colDiffuse;
uniform float progress;

void main()
{
    vec2 uv = fragTexCoord - 0.5;
    float dist = length(uv);
    float maxDist = 0.707;
    dist = dist / maxDist;
    float threshold = 1.0 - progress;
    float softness = 0.15;
    float vignette = smoothstep(threshold - softness, threshold + softness, dist);
    finalColor = vec4(0.0, 0.0, 0.0, vignette) * colDiffuse * fragColor;
}
