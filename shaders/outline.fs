#version 330

in vec2 fragTexCoord;
in vec4 fragColor;

uniform sampler2D texture0;
uniform vec2 resolution;
uniform float outlineThickness;
uniform vec3 outlineColor;

out vec4 finalColor;

// Sobel edge detection kernels
const mat3 sobelX = mat3(
    -1.0, 0.0, 1.0,
    -2.0, 0.0, 2.0,
    -1.0, 0.0, 1.0
);

const mat3 sobelY = mat3(
    -1.0, -2.0, -1.0,
     0.0,  0.0,  0.0,
     1.0,  2.0,  1.0
);

// Convert RGB to luminance
float luminance(vec3 color) {
    return dot(color, vec3(0.2126, 0.7152, 0.0722));
}

void main() {
    vec2 texelSize = outlineThickness / resolution;

    // Sample 3x3 neighborhood
    mat3 samples;
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            vec2 offset = vec2(float(i - 1), float(j - 1)) * texelSize;
            vec3 color = texture(texture0, fragTexCoord + offset).rgb;
            samples[i][j] = luminance(color);
        }
    }

    // Calculate gradients using Sobel operator
    float gx = 0.0;
    float gy = 0.0;

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            gx += samples[i][j] * sobelX[i][j];
            gy += samples[i][j] * sobelY[i][j];
        }
    }

    // Calculate gradient magnitude
    float g = sqrt(gx * gx + gy * gy);

    // Threshold for edge detection
    float edgeStrength = smoothstep(0.3, 0.7, g);

    // Get original color
    vec3 originalColor = texture(texture0, fragTexCoord).rgb;

    // Mix original with outline color based on edge strength
    vec3 result = mix(originalColor, outlineColor, edgeStrength);

    finalColor = vec4(result, 1.0);
}
