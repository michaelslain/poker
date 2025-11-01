#ifndef RENDER_UTILS_H
#define RENDER_UTILS_H

#include "raylib.h"

// Draw text as a billboard in 3D space (always faces camera)
void DrawTextBillboard(Camera3D camera, const char* text, Vector3 position, float fontSize, Color color);

// Draw text in 3D space at a fixed position (no billboarding)
void DrawText3D(const char* text, Vector3 position, float fontSize, Color color);

#endif
