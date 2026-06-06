#include "world-postprocess-shader.h"

#include <SDL2/SDL_opengl.h>

namespace {

using GlCreateShader = GLuint (APIENTRY *)(GLenum);
using GlShaderSource = void (APIENTRY *)(GLuint, GLsizei, const GLchar**, const GLint*);
using GlCompileShader = void (APIENTRY *)(GLuint);
using GlGetShaderiv = void (APIENTRY *)(GLuint, GLenum, GLint*);
using GlGetShaderInfoLog = void (APIENTRY *)(GLuint, GLsizei, GLsizei*, GLchar*);
using GlCreateProgram = GLuint (APIENTRY *)();
using GlAttachShader = void (APIENTRY *)(GLuint, GLuint);
using GlLinkProgram = void (APIENTRY *)(GLuint);
using GlGetProgramiv = void (APIENTRY *)(GLuint, GLenum, GLint*);
using GlGetProgramInfoLog = void (APIENTRY *)(GLuint, GLsizei, GLsizei*, GLchar*);
using GlUseProgram = void (APIENTRY *)(GLuint);
using GlGetUniformLocation = GLint (APIENTRY *)(GLuint, const GLchar*);
using GlUniform1i = void (APIENTRY *)(GLint, GLint);
using GlUniform1f = void (APIENTRY *)(GLint, GLfloat);
using GlUniform2f = void (APIENTRY *)(GLint, GLfloat, GLfloat);
using GlDeleteShader = void (APIENTRY *)(GLuint);

struct WorldShader {
    bool fTried = false;
    bool fReady = false;
    GLuint fProgram = 0;
    GLint fTex = -1;
    GLint fTexel = -1;
    GLint fSharpenLow = -1;
    GLint fSharpenHigh = -1;

    GlCreateShader createShader = nullptr;
    GlShaderSource shaderSource = nullptr;
    GlCompileShader compileShader = nullptr;
    GlGetShaderiv getShaderiv = nullptr;
    GlGetShaderInfoLog getShaderInfoLog = nullptr;
    GlCreateProgram createProgram = nullptr;
    GlAttachShader attachShader = nullptr;
    GlLinkProgram linkProgram = nullptr;
    GlGetProgramiv getProgramiv = nullptr;
    GlGetProgramInfoLog getProgramInfoLog = nullptr;
    GlUseProgram useProgram = nullptr;
    GlGetUniformLocation getUniformLocation = nullptr;
    GlUniform1i uniform1i = nullptr;
    GlUniform1f uniform1f = nullptr;
    GlUniform2f uniform2f = nullptr;
    GlDeleteShader deleteShader = nullptr;
};

WorldShader gWorldShader;
float gSharpenLow = 0.50f;
float gSharpenHigh = 0.155f;

template <class T>
T glProc(const char* const name) {
    return reinterpret_cast<T>(SDL_GL_GetProcAddress(name));
}

GLuint compileShader(WorldShader& s, const GLenum type, const char* const src) {
    const GLuint shader = s.createShader(type);
    const GLchar* source = src;
    s.shaderSource(shader, 1, &source, nullptr);
    s.compileShader(shader);
    GLint ok = 0;
    s.getShaderiv(shader, GL_COMPILE_STATUS, &ok);
    if(!ok) {
        return 0;
    }
    return shader;
}

bool initWorldShader() {
    auto& s = gWorldShader;
    if(s.fTried) return s.fReady;
    s.fTried = true;

    s.createShader = glProc<GlCreateShader>("glCreateShader");
    s.shaderSource = glProc<GlShaderSource>("glShaderSource");
    s.compileShader = glProc<GlCompileShader>("glCompileShader");
    s.getShaderiv = glProc<GlGetShaderiv>("glGetShaderiv");
    s.getShaderInfoLog = glProc<GlGetShaderInfoLog>("glGetShaderInfoLog");
    s.createProgram = glProc<GlCreateProgram>("glCreateProgram");
    s.attachShader = glProc<GlAttachShader>("glAttachShader");
    s.linkProgram = glProc<GlLinkProgram>("glLinkProgram");
    s.getProgramiv = glProc<GlGetProgramiv>("glGetProgramiv");
    s.getProgramInfoLog = glProc<GlGetProgramInfoLog>("glGetProgramInfoLog");
    s.useProgram = glProc<GlUseProgram>("glUseProgram");
    s.getUniformLocation = glProc<GlGetUniformLocation>("glGetUniformLocation");
    s.uniform1i = glProc<GlUniform1i>("glUniform1i");
    s.uniform1f = glProc<GlUniform1f>("glUniform1f");
    s.uniform2f = glProc<GlUniform2f>("glUniform2f");
    s.deleteShader = glProc<GlDeleteShader>("glDeleteShader");
    if(!s.createShader || !s.shaderSource || !s.compileShader ||
       !s.getShaderiv || !s.createProgram || !s.attachShader ||
       !s.linkProgram || !s.getProgramiv || !s.useProgram ||
       !s.getUniformLocation || !s.uniform1i || !s.uniform1f ||
       !s.uniform2f) {
        return false;
    }

    static const char* vert =
        "#version 110\n"
        "void main() {\n"
        "    gl_TexCoord[0] = gl_MultiTexCoord0;\n"
        "    gl_Position = ftransform();\n"
        "}\n";
    static const char* frag =
        "#version 110\n"
        "uniform sampler2D uTex;\n"
        "uniform vec2 uTexel;\n"
        "uniform float uSharpenLow;\n"
        "uniform float uSharpenHigh;\n"
        "const float PI = 3.14159265358979323846;\n"
        "float sinc(float x) {\n"
        "    x = abs(x);\n"
        "    if (x < 0.001) return 1.0;\n"
        "    x *= PI;\n"
        "    return sin(x) / x;\n"
        "}\n"
        "float lanczos2(float x) {\n"
        "    x = abs(x);\n"
        "    if (x >= 2.0) return 0.0;\n"
        "    return sinc(x) * sinc(x * 0.5);\n"
        "}\n"
        "void main() {\n"
        "    vec2 uv = gl_TexCoord[0].xy;\n"
        "    vec2 src = uv / uTexel;\n"
        "    vec2 base = floor(src - 0.5) + 0.5;\n"
        "    vec4 sum = vec4(0.0);\n"
        "    float weightSum = 0.0;\n"
        "    for (int y = -1; y <= 2; ++y) {\n"
        "        for (int x = -1; x <= 2; ++x) {\n"
        "            vec2 tap = base + vec2(float(x), float(y));\n"
        "            vec2 dist = src - tap;\n"
        "            float weight = lanczos2(dist.x) * lanczos2(dist.y);\n"
        "            sum += texture2D(uTex, tap * uTexel) * weight;\n"
        "            weightSum += weight;\n"
        "        }\n"
        "    }\n"
        "    vec4 color = clamp(sum / weightSum, 0.0, 1.0);\n"
        "    vec3 left = texture2D(uTex, uv - vec2(uTexel.x, 0.0)).rgb;\n"
        "    vec3 right = texture2D(uTex, uv + vec2(uTexel.x, 0.0)).rgb;\n"
        "    vec3 up = texture2D(uTex, uv - vec2(0.0, uTexel.y)).rgb;\n"
        "    vec3 down = texture2D(uTex, uv + vec2(0.0, uTexel.y)).rgb;\n"
        "    vec3 blur = (left + right + up + down) * 0.25;\n"
        "    vec3 mn = min(min(left, right), min(up, down));\n"
        "    vec3 mx = max(max(left, right), max(up, down));\n"
        "    float range = max(max(mx.r - mn.r, mx.g - mn.g), mx.b - mn.b);\n"
        "    float amount = mix(uSharpenLow, uSharpenHigh, clamp(range * 3.0, 0.0, 1.0));\n"
        "    vec3 sharp = color.rgb + (color.rgb - blur) * amount;\n"
        "    gl_FragColor = vec4(clamp(sharp, mn - 0.18, mx + 0.18), color.a);\n"
        "}\n";

    const GLuint vs = compileShader(s, GL_VERTEX_SHADER, vert);
    const GLuint fs = compileShader(s, GL_FRAGMENT_SHADER, frag);
    if(!vs || !fs) return false;

    s.fProgram = s.createProgram();
    s.attachShader(s.fProgram, vs);
    s.attachShader(s.fProgram, fs);
    s.linkProgram(s.fProgram);
    GLint ok = 0;
    s.getProgramiv(s.fProgram, GL_LINK_STATUS, &ok);
    if(s.deleteShader) {
        s.deleteShader(vs);
        s.deleteShader(fs);
    }
    if(!ok) {
        return false;
    }

    s.fTex = s.getUniformLocation(s.fProgram, "uTex");
    s.fTexel = s.getUniformLocation(s.fProgram, "uTexel");
    s.fSharpenLow = s.getUniformLocation(s.fProgram, "uSharpenLow");
    s.fSharpenHigh = s.getUniformLocation(s.fProgram, "uSharpenHigh");
    s.fReady = true;
    return true;
}

}

bool applyWorldPostprocessShader(SDL_Renderer* const r,
                                 SDL_Texture* const worldTex,
                                 const SDL_Rect& srcRect,
                                 const SDL_Rect& dstRect,
                                 const int worldW,
                                 const int worldH) {
    if(!initWorldShader()) return false;
    SDL_RenderFlush(r);

    float texW = 1.f;
    float texH = 1.f;
    if(SDL_GL_BindTexture(worldTex, &texW, &texH) != 0) {
        return false;
    }

    auto& s = gWorldShader;
    int outW = 0;
    int outH = 0;
    SDL_GetRendererOutputSize(r, &outW, &outH);

    glPushAttrib(GL_ENABLE_BIT | GL_VIEWPORT_BIT | GL_COLOR_BUFFER_BIT |
                 GL_SCISSOR_BIT);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glDisable(GL_BLEND);
    glDisable(GL_SCISSOR_TEST);
    glViewport(0, 0, outW, outH);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    s.useProgram(s.fProgram);
    s.uniform1i(s.fTex, 0);
    s.uniform2f(s.fTexel, texW / worldW, texH / worldH);
    s.uniform1f(s.fSharpenLow, gSharpenLow);
    s.uniform1f(s.fSharpenHigh, gSharpenHigh);

    const GLfloat u0 = texW * srcRect.x / GLfloat(worldW);
    const GLfloat v0 = texH * srcRect.y / GLfloat(worldH);
    const GLfloat u1 = texW * (srcRect.x + srcRect.w) / GLfloat(worldW);
    const GLfloat v1 = texH * (srcRect.y + srcRect.h) / GLfloat(worldH);

    const GLfloat x0 = -1.f + 2.f * dstRect.x / GLfloat(outW);
    const GLfloat x1 = -1.f + 2.f * (dstRect.x + dstRect.w) / GLfloat(outW);
    const GLfloat y0 = 1.f - 2.f * dstRect.y / GLfloat(outH);
    const GLfloat y1 = 1.f - 2.f * (dstRect.y + dstRect.h) / GLfloat(outH);

    glBegin(GL_QUADS);
    glTexCoord2f(u0, v0); glVertex2f(x0, y0);
    glTexCoord2f(u1, v0); glVertex2f(x1, y0);
    glTexCoord2f(u1, v1); glVertex2f(x1, y1);
    glTexCoord2f(u0, v1); glVertex2f(x0, y1);
    glEnd();

    s.useProgram(0);
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopAttrib();
    SDL_GL_UnbindTexture(worldTex);
    return true;
}

void adjustWorldPostprocessSharpen(const float delta) {
    gSharpenLow += delta;
    if(gSharpenLow < 0.f) gSharpenLow = 0.f;
    if(gSharpenLow > 4.f) gSharpenLow = 4.f;
    gSharpenHigh = gSharpenLow * 0.31f;
}

float worldPostprocessSharpenLow() {
    return gSharpenLow;
}

float worldPostprocessSharpenHigh() {
    return gSharpenHigh;
}
