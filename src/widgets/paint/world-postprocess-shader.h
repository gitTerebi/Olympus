#ifndef WORLD_POSTPROCESS_SHADER_H
#define WORLD_POSTPROCESS_SHADER_H

#include <SDL2/SDL_render.h>

bool applyWorldPostprocessShader(SDL_Renderer* r,
                                 SDL_Texture* worldTex,
                                 const SDL_Rect& srcRect,
                                 const SDL_Rect& dstRect,
                                 int worldW,
                                 int worldH);

void adjustWorldPostprocessSharpen(float delta);
float worldPostprocessSharpenLow();
float worldPostprocessSharpenHigh();

#endif // WORLD_POSTPROCESS_SHADER_H
