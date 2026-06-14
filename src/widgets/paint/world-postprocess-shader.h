#ifndef WORLD_POSTPROCESS_SHADER_H
#define WORLD_POSTPROCESS_SHADER_H

#include <SDL2/SDL_render.h>

// Whole-frame upscale pass (Direct3D 11 + HLSL), DisciplesGL-style.
//
// The game renders its whole frame (world + UI) into frameTex at the configured
// resolution. This pass runs an optional pixel-art UPSCALE (xBRZ / ScaleHQ / ScaleNx
// / Eagle / xSal at Nx into an intermediate render target), then an INTERPOLATION
// resample (nearest / linear / hermite / cubic / lanczos) to fill the current window,
// letterboxed to the frame's aspect. All filters are ported from the DisciplesGL GLSL.
//
// Returns false on any failure (non-D3D11 renderer, layout mismatch) so the caller
// can fall back to a plain copy.

// interpolation: eInterpolation as int; upscale: eUpscale as int; factor: 2..6.
void setPostprocessFilters(int interpolation, int upscale, int factor);

bool applyFullFramePostprocess(SDL_Renderer* r,
                               const void* pixels,
                               int pitch,
                               int frameW,
                               int frameH);

#endif // WORLD_POSTPROCESS_SHADER_H
