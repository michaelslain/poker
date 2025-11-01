#version 330

in vec2 fragTexCoord;
in vec4 fragColor;

uniform sampler2D texture0;
uniform vec2 resolution;
uniform float brightnessBoost;      // How much to brighten the highlighted object (e.g., 0.3)
uniform float vignetteStrength;     // Strength of the vignette effect (e.g., 0.5)
uniform float vignetteRadius;       // Radius of the vignette (e.g., 0.7)

out vec4 finalColor;

void main() {
    // Get original color
    vec3 color = texture(texture0, fragTexCoord).rgb;

    // Calculate vignette effect
    // Distance from center of screen
    vec2 uv = fragTexCoord - 0.5;  // Center at (0, 0)
    float dist = length(uv);

    // Create vignette falloff
    float vignette = smoothstep(vignetteRadius, vignetteRadius - 0.3, dist);

    // Apply vignette (darken edges)
    color = mix(color * (1.0 - vignetteStrength), color, vignette);

    // Apply brightness boost for highlighted objects
    // This will be controlled by the uniform, set to 0.0 for non-highlighted objects
    color += vec3(brightnessBoost);

    finalColor = vec4(color, 1.0);
}
