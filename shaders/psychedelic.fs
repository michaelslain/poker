#version 330

// Input vertex attributes (from vertex shader)
in vec2 fragTexCoord;
in vec4 fragColor;
in vec3 fragPosition;
in vec3 fragNormal;

// Input uniform values
uniform sampler2D texture0;
uniform vec4 colDiffuse;

// Psychedelic effect uniforms
uniform float time;
uniform float intensity;  // 0.0 to 1.0

// Output fragment color
out vec4 finalColor;

// Simplex noise functions
vec3 mod289(vec3 x) { return x - floor(x * (1.0 / 289.0)) * 289.0; }
vec2 mod289(vec2 x) { return x - floor(x * (1.0 / 289.0)) * 289.0; }
vec3 permute(vec3 x) { return mod289(((x*34.0)+1.0)*x); }

float snoise(vec2 v) {
    const vec4 C = vec4(0.211324865405187,  // (3.0-sqrt(3.0))/6.0
                        0.366025403784439,  // 0.5*(sqrt(3.0)-1.0)
                       -0.577350269189626,  // -1.0 + 2.0 * C.x
                        0.024390243902439); // 1.0 / 41.0
    vec2 i  = floor(v + dot(v, C.yy) );
    vec2 x0 = v -   i + dot(i, C.xx);
    vec2 i1;
    i1 = (x0.x > x0.y) ? vec2(1.0, 0.0) : vec2(0.0, 1.0);
    vec4 x12 = x0.xyxy + C.xxzz;
    x12.xy -= i1;
    i = mod289(i);
    vec3 p = permute( permute( i.y + vec3(0.0, i1.y, 1.0 ))
        + i.x + vec3(0.0, i1.x, 1.0 ));
    vec3 m = max(0.5 - vec3(dot(x0,x0), dot(x12.xy,x12.xy), dot(x12.zw,x12.zw)), 0.0);
    m = m*m ;
    m = m*m ;
    vec3 x = 2.0 * fract(p * C.www) - 1.0;
    vec3 h = abs(x) - 0.5;
    vec3 ox = floor(x + 0.5);
    vec3 a0 = x - ox;
    m *= 1.79284291400159 - 0.85373472095314 * ( a0*a0 + h*h );
    vec3 g;
    g.x  = a0.x  * x0.x  + h.x  * x0.y;
    g.yz = a0.yz * x12.xz + h.yz * x12.yw;
    return 130.0 * dot(m, g);
}

// RGB to HSV conversion
vec3 rgb2hsv(vec3 c) {
    vec4 K = vec4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);
    vec4 p = mix(vec4(c.bg, K.wz), vec4(c.gb, K.xy), step(c.b, c.g));
    vec4 q = mix(vec4(p.xyw, c.r), vec4(c.r, p.yzx), step(p.x, c.r));
    float d = q.x - min(q.w, q.y);
    float e = 1.0e-10;
    return vec3(abs(q.z + (q.w - q.y) / (6.0 * d + e)), d / (q.x + e), q.x);
}

// HSV to RGB conversion
vec3 hsv2rgb(vec3 c) {
    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

// Polar coordinates helper
vec2 toPolar(vec2 uv, vec2 center) {
    vec2 delta = uv - center;
    float radius = length(delta);
    float angle = atan(delta.y, delta.x);
    return vec2(radius, angle);
}

// Fractal Brownian Motion noise
float fbm(vec2 p) {
    float value = 0.0;
    float amplitude = 0.5;
    float frequency = 1.0;
    for (int i = 0; i < 4; i++) {
        value += amplitude * snoise(p * frequency);
        frequency *= 2.0;
        amplitude *= 0.5;
    }
    return value;
}

void main()
{
    vec2 uv = fragTexCoord;
    vec4 texelColor = texture(texture0, uv);

    if (intensity < 0.01) {
        // No effect
        finalColor = texelColor * colDiffuse * fragColor;
        return;
    }

    // Calculate trip stage based on time and intensity
    // Stage 0-1: Come up (0-60s)
    // Stage 1-2: Peak (60-180s)
    // Stage 2-3: Come down (180-300s)
    float tripDuration = 300.0; // 5 minutes total
    float comeUpEnd = 60.0;
    float peakEnd = 180.0;

    float stage = 0.0;
    float localIntensity = intensity;

    if (time < comeUpEnd) {
        // Come up phase: intensity ramps 0 -> 1
        stage = time / comeUpEnd;
        localIntensity = intensity * smoothstep(0.0, 1.0, stage);
    } else if (time < peakEnd) {
        // Peak phase: full intensity with waves
        stage = 1.0 + (time - comeUpEnd) / (peakEnd - comeUpEnd);
        float wave = sin(time * 0.5) * 0.15 + 0.85; // Oscillate 0.7-1.0
        localIntensity = intensity * wave;
    } else if (time < tripDuration) {
        // Come down phase: intensity ramps 1 -> 0
        stage = 2.0 + (time - peakEnd) / (tripDuration - peakEnd);
        float comeDown = 1.0 - smoothstep(0.0, 1.0, (time - peakEnd) / (tripDuration - peakEnd));
        localIntensity = intensity * comeDown;
    } else {
        // Trip over
        finalColor = texelColor * colDiffuse * fragColor;
        return;
    }

    // ===== EFFECT 1: Breathing/Morphing Walls =====
    float breathe = sin(time * 2.0) * 0.5 + 0.5; // 0-1 oscillation
    float warp = breathe * localIntensity * 0.05;

    vec2 warpedUV = uv;
    warpedUV.x += sin(uv.y * 10.0 + time * 2.0) * warp;
    warpedUV.y += cos(uv.x * 10.0 + time * 2.0) * warp;

    // ===== EFFECT 2: Drifting/Warping with Noise =====
    float noiseScale = 5.0 + stage * 5.0; // Increase detail over time
    vec2 noiseOffset = vec2(
        fbm(warpedUV * noiseScale + time * 0.3) * localIntensity * 0.1,
        fbm(warpedUV * noiseScale + time * 0.3 + 100.0) * localIntensity * 0.1
    );
    warpedUV += noiseOffset;

    // ===== EFFECT 3: Geometric Patterns (Spirals & Tunnels) =====
    vec2 center = vec2(0.5, 0.5);
    vec2 polar = toPolar(warpedUV, center);
    float radius = polar.x;
    float angle = polar.y;

    // Spiral distortion
    float spiralStrength = localIntensity * 0.3;
    float spiral = sin(radius * 20.0 - angle * 3.0 - time * 2.0) * spiralStrength;
    warpedUV += normalize(warpedUV - center) * spiral * 0.05;

    // Tunnel effect
    if (stage > 1.0) { // More intense during peak
        float tunnel = sin(radius * 15.0 - time * 3.0) * 0.5 + 0.5;
        warpedUV = mix(warpedUV, center + (warpedUV - center) * (1.0 + tunnel * localIntensity * 0.3), localIntensity * 0.5);
    }

    // Sample texture with warped coordinates
    texelColor = texture(texture0, warpedUV);

    // ===== EFFECT 4: Color Shifting =====
    vec3 hsv = rgb2hsv(texelColor.rgb);

    // Hue rotation - speed increases with stage
    float hueShift = time * (0.1 + stage * 0.1) + fbm(uv * 5.0) * localIntensity * 0.5;
    hsv.x = fract(hsv.x + hueShift * localIntensity);

    // Saturation boost
    hsv.y = clamp(hsv.y + localIntensity * 0.3, 0.0, 1.0);

    // Brightness oscillation
    float brightnessWave = sin(time * 1.5 + uv.x * 10.0) * 0.1 + 0.9;
    hsv.z *= mix(1.0, brightnessWave, localIntensity * 0.5);

    vec3 shiftedColor = hsv2rgb(hsv);

    // ===== EFFECT 5: Geometric Pattern Overlay =====
    if (stage > 0.5) {
        // Lattice pattern
        float latticeX = sin(warpedUV.x * 30.0 + time) * 0.5 + 0.5;
        float latticeY = sin(warpedUV.y * 30.0 - time) * 0.5 + 0.5;
        float lattice = latticeX * latticeY;

        // Cobweb pattern
        float cobweb = sin(radius * 40.0) * sin(angle * 8.0);
        cobweb = cobweb * 0.5 + 0.5;

        float patternMix = mix(lattice, cobweb, sin(time * 0.5) * 0.5 + 0.5);
        vec3 patternColor = hsv2rgb(vec3(fract(time * 0.2 + radius), 0.8, 0.8));

        float patternStrength = (stage - 0.5) * localIntensity * 0.3;
        shiftedColor = mix(shiftedColor, patternColor, patternMix * patternStrength);
    }

    // ===== EFFECT 6: Edge Glow (Peak only) =====
    if (stage > 1.0 && stage < 2.0) {
        float edgeGlow = 1.0 - smoothstep(0.0, 0.5, length(uv - center));
        vec3 glowColor = hsv2rgb(vec3(fract(time * 0.3), 1.0, 1.0));
        shiftedColor += glowColor * edgeGlow * localIntensity * 0.2;
    }

    // Final color assembly
    finalColor = vec4(shiftedColor, texelColor.a) * colDiffuse * fragColor;
}
