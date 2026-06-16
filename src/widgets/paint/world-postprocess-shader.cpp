#include "world-postprocess-shader.h"

#include <SDL2/SDL_system.h>

#ifdef _WIN32
#include <d3d11.h>
#include <d3dcompiler.h>
#endif

#include <cstdint>
#include <cstdio>
#include <cstring>
#include <map>
#include <string>

#ifdef _WIN32
// ---------------------------------------------------------------------------
// Whole-frame upscale (Direct3D 11 + HLSL). Two stages, DisciplesGL parity:
//   1. optional pixel-art upscale (frame -> Nx intermediate RT)
//   2. interpolation resample (-> backbuffer, letterboxed to frame aspect)
//
// SDL owns the 2D game renderer, but this postprocess path owns its shader source
// texture. The frame is read from SDL's render target and uploaded into a D3D11 SRV
// we control, then the shader pass writes to SDL's D3D11 swapchain RTV.
// ---------------------------------------------------------------------------
namespace {

struct SdlDRectMirror {
    double x;
    double y;
    double w;
    double h;
};

struct SdlRendererMirror {
    const void* magic;
    void* funcs[29];
    SDL_RendererInfo info;
    SDL_Window* window;
    SDL_bool hidden;
    SDL_bool wanted_vsync;
    SDL_bool simulate_vsync;
    Uint32 simulate_vsync_interval;
    Uint32 last_present;
    int logical_w;
    int logical_h;
    int logical_w_backup;
    int logical_h_backup;
    SDL_bool integer_scale;
    SdlDRectMirror viewport;
    SdlDRectMirror viewport_backup;
    SdlDRectMirror clip_rect;
    SdlDRectMirror clip_rect_backup;
    SDL_bool clipping_enabled;
    SDL_bool clipping_enabled_backup;
    SDL_FPoint scale;
    SDL_FPoint scale_backup;
    SDL_FPoint dpi_scale;
    SDL_bool relative_scaling;
    int line_method;
    int rect_index_order[6];
    float xrel;
    float yrel;
    SDL_Texture* textures;
    SDL_Texture* target;
    void* target_mutex;
    SDL_Color color;
    SDL_BlendMode blendMode;
    SDL_bool always_batch;
    SDL_bool batching;
    void* render_commands;
    void* render_commands_tail;
    void* render_commands_pool;
    Uint32 render_command_generation;
    Uint32 last_queued_color;
    SdlDRectMirror last_queued_viewport;
    SdlDRectMirror last_queued_cliprect;
    SDL_bool last_queued_cliprect_enabled;
    SDL_bool color_queued;
    SDL_bool viewport_queued;
    SDL_bool cliprect_queued;
    void* vertex_data;
    size_t vertex_data_used;
    size_t vertex_data_allocation;
    void* driverdata;
};

struct D3D11RenderDataHead {
    void* hDXGIMod;
    void* hD3D11Mod;
    void* dxgiFactory;
    void* dxgiAdapter;
    void* d3dDevice;
    void* d3dContext;
    void* swapChain;
    DXGI_SWAP_EFFECT swapEffect;
    ID3D11RenderTargetView* mainRenderTargetView;
    ID3D11RenderTargetView* currentOffscreenRenderTargetView;
    void* inputLayout;
    void* vertexBuffers[8];
    size_t vertexBufferSizes[8];
    void* vertexShader;
    void* pixelShaders[16];
    int blendModesCount;
    void* blendModes;
    void* nearestPixelSampler;
    void* linearSampler;
    D3D_FEATURE_LEVEL featureLevel;
    void* mainRasterizer;
    void* clippedRasterizer;
    char vertexShaderConstantsData[128];
    void* vertexShaderConstants;
    int rotation;
    ID3D11RenderTargetView* currentRenderTargetView;
    ID3D11RasterizerState* currentRasterizerState;
    ID3D11BlendState* currentBlendState;
    ID3D11PixelShader* currentShader;
    ID3D11ShaderResourceView* currentShaderResource;
    ID3D11SamplerState* currentSampler;
};

struct SdlTextureMirror {
    const void* magic;
    Uint32 format;
    int access;
    int w;
    int h;
    int modMode;
    SDL_BlendMode blendMode;
    SDL_ScaleMode scaleMode;
    SDL_Color color;
    SDL_Renderer* renderer;
    SDL_Texture* native;
    void* yuv;
    void* pixels;
    int pitch;
    SDL_Rect lockedRect;
    void* lockedSurface;
    Uint32 lastCommandGeneration;
    void* driverdata;
    void* userdata;
    SDL_Texture* prev;
    SDL_Texture* next;
};

struct D3D11TextureDataHead {
    ID3D11Texture2D* mainTexture;
    ID3D11ShaderResourceView* mainTextureResourceView;
    ID3D11RenderTargetView* mainTextureRenderTargetView;
};

ID3D11RenderTargetView* mainRtv(SDL_Renderer* const r, void** const outData) {
    if(outData) *outData = nullptr;
    if(!r) return nullptr;
    const auto rm = reinterpret_cast<const SdlRendererMirror*>(r);
    if(outData) *outData = rm->driverdata;
    if(!rm->driverdata) return nullptr;
    const auto data = reinterpret_cast<D3D11RenderDataHead*>(rm->driverdata);
    return data->mainRenderTargetView;
}

ID3D11ShaderResourceView* textureSrv(SDL_Texture* const tex) {
    if(!tex) return nullptr;
    auto tm = reinterpret_cast<const SdlTextureMirror*>(tex);
    if(!tm->driverdata && tm->native) {
        tm = reinterpret_cast<const SdlTextureMirror*>(tm->native);
    }
    if(!tm->driverdata) {
        static bool once = false;
        if(!once) {
            once = true;
            printf("post-process: SDL texture has no D3D11 driverdata\n");
        }
        return nullptr;
    }
    const auto data = reinterpret_cast<D3D11TextureDataHead*>(tm->driverdata);
    if(!data->mainTextureResourceView) {
        static bool once = false;
        if(!once) {
            once = true;
            printf("post-process: SDL D3D11 texture has no SRV\n");
        }
    }
    return data->mainTextureResourceView;
}

void dirtySdlD3D11State(void* const rendererData) {
    if(!rendererData) return;
    auto* const data = reinterpret_cast<D3D11RenderDataHead*>(rendererData);
    data->currentRenderTargetView = nullptr;
    data->currentRasterizerState = nullptr;
    data->currentBlendState = nullptr;
    data->currentShader = nullptr;
    data->currentShaderResource = nullptr;
    data->currentSampler = nullptr;
}

// ---------------------------------------------------------------------------
// HLSL. cb: float2 uTexel (1/srcSize), float2 uSrcSize. Sampler s0.
// All filters ported from DisciplesGL src/glsl. uv is normalized [0,1].
// ---------------------------------------------------------------------------
const char* const kVs =
    "struct VSOut { float4 pos : SV_Position; float2 uv : TEXCOORD0; };\n"
    "VSOut main(uint id : SV_VertexID) {\n"
    "    VSOut o;\n"
    "    float2 uv = float2((id << 1) & 2, id & 2);\n"
    "    o.uv = uv;\n"
    "    o.pos = float4(uv.x * 2.0 - 1.0, 1.0 - uv.y * 2.0, 0.0, 1.0);\n"
    "    return o;\n"
    "}\n";

const char* const kCbHead =
    "Texture2D uTex : register(t0);\n"
    "SamplerState uSamp : register(s0);\n"
    "cbuffer Cb : register(b0) { float2 uTexel; float2 uSrcSize; };\n";

// --- interpolation: nearest / linear (sampler does the work) ---
const char* const kBlitPs =
    "Texture2D uTex : register(t0);\n"
    "SamplerState uSamp : register(s0);\n"
    "cbuffer Cb : register(b0) { float2 uTexel; float2 uSrcSize; };\n"
    "float4 main(float4 pos : SV_Position, float2 uv : TEXCOORD0) : SV_Target {\n"
    "    return float4(uTex.SampleLevel(uSamp, uv, 0.0).rgb, 1.0);\n"
    "}\n";

// --- interpolation: hermite (smoothstep texel interpolation) ---
const char* const kHermitePs =
    "Texture2D uTex : register(t0);\n"
    "SamplerState uSamp : register(s0);\n"
    "cbuffer Cb : register(b0) { float2 uTexel; float2 uSrcSize; };\n"
    "float4 main(float4 pos : SV_Position, float2 uvIn : TEXCOORD0) : SV_Target {\n"
    "    float2 uv = uvIn * uSrcSize - 0.5;\n"
    "    float2 texel = floor(uv) + 0.5;\n"
    "    float2 t = frac(uv);\n"
    "    uv = texel + t * t * t * (t * (t * 6.0 - 15.0) + 10.0);\n"
    "    return uTex.SampleLevel(uSamp, uv * uTexel, 0.0);\n"
    "}\n";

// --- interpolation: cubic (Catmull-Rom, DisciplesGL cubic) ---
const char* const kCubicPs =
    "Texture2D uTex : register(t0);\n"
    "SamplerState uSamp : register(s0);\n"
    "cbuffer Cb : register(b0) { float2 uTexel; float2 uSrcSize; };\n"
    "float4 main(float4 pos : SV_Position, float2 uv : TEXCOORD0) : SV_Target {\n"
    "    const float4 p0 = float4(0.0, -0.5,  1.0, -0.5);\n"
    "    const float4 p1 = float4(1.0,  0.0, -2.5,  1.5);\n"
    "    const float4 p2 = float4(0.0,  0.5,  2.0, -1.5);\n"
    "    const float4 p3 = float4(0.0,  0.0, -0.5,  0.5);\n"
    "    float2 c = uv * uSrcSize - 0.5;\n"
    "    float2 texel = floor(c) - 0.5;\n"
    "    float2 t = frac(c);\n"
    "    float2 t2 = t * t; float2 t3 = t2 * t;\n"
    "    float4 xv = float4(1.0, t.x, t2.x, t3.x);\n"
    "    xv = float4(dot(xv,p0), dot(xv,p1), dot(xv,p2), dot(xv,p3));\n"
    "    float4 yv = float4(1.0, t.y, t2.y, t3.y);\n"
    "    yv = float4(dot(yv,p0), dot(yv,p1), dot(yv,p2), dot(yv,p3));\n"
    "    #define TEX(a,b) uTex.SampleLevel(uSamp, (texel + float2(a,b)) * uTexel, 0.0)\n"
    "    return\n"
    "        (TEX(0,0)*xv.x + TEX(1,0)*xv.y + TEX(2,0)*xv.z + TEX(3,0)*xv.w) * yv.x +\n"
    "        (TEX(0,1)*xv.x + TEX(1,1)*xv.y + TEX(2,1)*xv.z + TEX(3,1)*xv.w) * yv.y +\n"
    "        (TEX(0,2)*xv.x + TEX(1,2)*xv.y + TEX(2,2)*xv.z + TEX(3,2)*xv.w) * yv.z +\n"
    "        (TEX(0,3)*xv.x + TEX(1,3)*xv.y + TEX(2,3)*xv.z + TEX(3,3)*xv.w) * yv.w;\n"
    "}\n";

// --- interpolation: lanczos (DisciplesGL lanczos) ---
const char* const kLanczosPs =
    "Texture2D uTex : register(t0);\n"
    "SamplerState uSamp : register(s0);\n"
    "cbuffer Cb : register(b0) { float2 uTexel; float2 uSrcSize; };\n"
    "static const float PI = 3.14159265358979323846;\n"
    "float3 wfn(float a) {\n"
    "    float3 s = max(abs(2.0 * PI * float3(a - 1.5, a - 0.5, a + 0.5)), 1e-5);\n"
    "    return sin(s) * sin(s / 3.0) / (s * s);\n"
    "}\n"
    "float4 lumf(float3 x1, float3 x2, float y, float3 y1, float3 y2) {\n"
    "    #define TX(a) uTex.SampleLevel(uSamp, float2(a, y), 0.0)\n"
    "    return TX(x1.x)*y1.x + TX(x1.y)*y1.y + TX(x1.z)*y1.z +\n"
    "           TX(x2.x)*y2.x + TX(x2.y)*y2.y + TX(x2.z)*y2.z;\n"
    "}\n"
    "float4 main(float4 pos : SV_Position, float2 uvIn : TEXCOORD0) : SV_Target {\n"
    "    float2 stp = uTexel;\n"
    "    float2 uv = uvIn + stp * 0.5;\n"
    "    float2 f = frac(uv / stp);\n"
    "    float3 y1 = wfn(0.5 - f.x*0.5); float3 y2 = wfn(1.0 - f.x*0.5);\n"
    "    float3 x1 = wfn(0.5 - f.y*0.5); float3 x2 = wfn(1.0 - f.y*0.5);\n"
    "    float3 one = float3(1,1,1);\n"
    "    float xsum = dot(x1,one)+dot(x2,one); float ysum = dot(y1,one)+dot(y2,one);\n"
    "    x1/=xsum; x2/=xsum; y1/=ysum; y2/=ysum;\n"
    "    float2 p = (-0.5 - f) * stp + uv;\n"
    "    float3 px1 = float3(p.x-stp.x*2.0, p.x, p.x+stp.x*2.0);\n"
    "    float3 px2 = float3(p.x-stp.x, p.x+stp.x, p.x+stp.x*3.0);\n"
    "    float3 py1 = float3(p.y-stp.y*2.0, p.y, p.y+stp.y*2.0);\n"
    "    float3 py2 = float3(p.y-stp.y, p.y+stp.y, p.y+stp.y*3.0);\n"
    "    #define LM(a) lumf(px1, px2, a, y1, y2)\n"
    "    return LM(py1.x)*x1.x + LM(py1.y)*x1.y + LM(py1.z)*x1.z +\n"
    "           LM(py2.x)*x2.x + LM(py2.y)*x2.y + LM(py2.z)*x2.z;\n"
    "}\n";

// --- upscale: ScaleNx 2x (DisciplesGL scalenx 2x) ---
const char* const kScaleNx2Ps =
    "Texture2D uTex : register(t0);\n"
    "SamplerState uSamp : register(s0);\n"
    "cbuffer Cb : register(b0) { float2 uTexel; float2 uSrcSize; };\n"
    "float4 TX(float2 texel, float x, float y) { return uTex.SampleLevel(uSamp, (texel + float2(x,y)) * uTexel, 0.0); }\n"
    "float4 main(float4 pos : SV_Position, float2 uv : TEXCOORD0) : SV_Target {\n"
    "    float2 texel = floor(uv * uSrcSize) + 0.5;\n"
    "    float4 B = TX(texel, 0.0,-1.0);\n"
    "    float4 D = TX(texel,-1.0, 0.0);\n"
    "    float4 E = TX(texel, 0.0, 0.0);\n"
    "    float4 F = TX(texel, 1.0, 0.0);\n"
    "    float4 H = TX(texel, 0.0, 1.0);\n"
    "    float4 E0 = all(B==D) ? B : E;\n"
    "    float4 E1 = all(B==F) ? B : E;\n"
    "    float4 E2 = all(H==D) ? H : E;\n"
    "    float4 E3 = all(H==F) ? H : E;\n"
    "    float2 fp = floor(2.0 * frac(uv * uSrcSize));\n"
    "    bool edge = any(B!=H) && any(D!=F);\n"
    "    if(!edge) return E;\n"
    "    return fp.y==0.0 ? (fp.x==0.0 ? E0 : E1) : (fp.x==0.0 ? E2 : E3);\n"
    "}\n";

// --- upscale: ScaleNx 3x (DisciplesGL scalenx 3x) ---
const char* const kScaleNx3Ps =
    "Texture2D uTex : register(t0);\n"
    "SamplerState uSamp : register(s0);\n"
    "cbuffer Cb : register(b0) { float2 uTexel; float2 uSrcSize; };\n"
    "float4 TX(float2 texel, float x, float y) { return uTex.SampleLevel(uSamp, (texel + float2(x,y)) * uTexel, 0.0); }\n"
    "float4 main(float4 pos : SV_Position, float2 uv : TEXCOORD0) : SV_Target {\n"
    "    float2 texel = floor(uv * uSrcSize) + 0.5;\n"
    "    float4 A=TX(texel,-1,-1),B=TX(texel,0,-1),C=TX(texel,1,-1);\n"
    "    float4 D=TX(texel,-1,0), E=TX(texel,0,0), F=TX(texel,1,0);\n"
    "    float4 G=TX(texel,-1,1), H=TX(texel,0,1), I=TX(texel,1,1);\n"
    "    bool eqBD=all(B==D),eqBF=all(B==F),eqHD=all(H==D),eqHF=all(H==F);\n"
    "    bool neqEA=any(E!=A),neqEC=any(E!=C),neqEG=any(E!=G),neqEI=any(E!=I);\n"
    "    float4 E0 = eqBD ? B : E;\n"
    "    float4 E1 = ((eqBD&&neqEC)||(eqBF&&neqEA)) ? B : E;\n"
    "    float4 E2 = eqBF ? B : E;\n"
    "    float4 E3 = ((eqBD&&neqEG)||(eqHD&&neqEA)) ? D : E;\n"
    "    float4 E5 = ((eqBF&&neqEI)||(eqHF&&neqEC)) ? F : E;\n"
    "    float4 E6 = eqHD ? H : E;\n"
    "    float4 E7 = ((eqHD&&neqEI)||(eqHF&&neqEG)) ? H : E;\n"
    "    float4 E8 = eqHF ? H : E;\n"
    "    float2 fp = floor(3.0 * frac(uv * uSrcSize));\n"
    "    bool edge = any(B!=H) && any(D!=F);\n"
    "    if(!edge) return E;\n"
    "    if(fp.y==0.0) return fp.x==0.0?E0:(fp.x==1.0?E1:E2);\n"
    "    if(fp.y==1.0) return fp.x==0.0?E3:(fp.x==1.0?E:E5);\n"
    "    return fp.x==0.0?E6:(fp.x==1.0?E7:E8);\n"
    "}\n";

// --- upscale: Eagle 2x (DisciplesGL eagle) ---
const char* const kEaglePs =
    "Texture2D uTex : register(t0);\n"
    "SamplerState uSamp : register(s0);\n"
    "cbuffer Cb : register(b0) { float2 uTexel; float2 uSrcSize; };\n"
    "static const float4 dtt = float4(65536.0,255.0,1.0,0.0);\n"
    "float reduce(float4 c){ return dot(c, dtt); }\n"
    "float4 TX(float2 texel, float x, float y){ return uTex.SampleLevel(uSamp,(texel+float2(x,y))*uTexel,0.0); }\n"
    "int GR(float A,float B,float C,float D){ int x=0,y=0,r=0; if(A==C)x+=1; else if(B==C)y+=1; if(A==D)x+=1; else if(B==D)y+=1; if(x<=1)r+=1; if(y<=1)r-=1; return r; }\n"
    "float4 main(float4 pos : SV_Position, float2 uv : TEXCOORD0) : SV_Target {\n"
    "    float2 texel = floor(uv * uSrcSize) + 0.5;\n"
    "    float4 C1=TX(texel,-1,0),C2=TX(texel,1,-1),D3=TX(texel,2,-1);\n"
    "    float4 C3=TX(texel,-1,0),C4=TX(texel,0,0),C5=TX(texel,1,0),D4=TX(texel,2,0);\n"
    "    float4 C6=TX(texel,-1,1),C7=TX(texel,0,1),C8=TX(texel,1,1);\n"
    "    float4 D5=TX(texel,0,1),D0=TX(texel,1,1),D1=TX(texel,0,2),D2=TX(texel,1,2);\n"
    "    float4 p00,p10,p01,p11;\n"
    "    float c1=reduce(C1),c2=reduce(C2),c3=reduce(C3),c4=reduce(C4),c5=reduce(C5);\n"
    "    float c6=reduce(C6),c7=reduce(C7),c8=reduce(C8),d1=reduce(D1),d2=reduce(D2);\n"
    "    float d4=reduce(D4),d5=reduce(D5);\n"
    "    if(c4!=c8){\n"
    "        if(c7==c5){ p01=p10=C7;\n"
    "            p00 = (c6==c7||c5==c2) ? 0.25*(3.0*C7+C4) : 0.5*(C4+C5);\n"
    "            p11 = (c5==d4||c7==d1) ? 0.25*(3.0*C7+C8) : 0.5*(C7+C8);\n"
    "        } else { p11=0.125*(6.0*C8+C7+C5); p00=0.125*(6.0*C4+C7+C5);\n"
    "                 p10=0.125*(6.0*C7+C4+C8); p01=0.125*(6.0*C5+C4+C8); }\n"
    "    } else if(c7!=c5){ p11=p00=C4;\n"
    "        p01 = (c1==c4||c8==d5) ? 0.25*(3.0*C4+C5) : 0.5*(C4+C5);\n"
    "        p10 = (c8==d2||c3==c4) ? 0.25*(3.0*C4+C7) : 0.5*(C7+C8);\n"
    "    } else {\n"
    "        int r = GR(c5,c4,c6,d1)+GR(c5,c4,c3,c1)+GR(c5,c4,d2,d5)+GR(c5,c4,c2,d4);\n"
    "        if(r>0){ p01=p10=C7; p00=p11=0.5*(C4+C5); }\n"
    "        else if(r<0){ p11=p00=C4; p01=p10=0.5*(C4+C5); }\n"
    "        else { p11=p00=C4; p01=p10=C7; }\n"
    "    }\n"
    "    float2 fp = frac(uv * uSrcSize);\n"
    "    return fp.x<0.5 ? (fp.y<0.5?p00:p10) : (fp.y<0.5?p01:p11);\n"
    "}\n";

// --- upscale: xSal 2x (DisciplesGL xsal) ---
const char* const kXSalPs =
    "Texture2D uTex : register(t0);\n"
    "SamplerState uSamp : register(s0);\n"
    "cbuffer Cb : register(b0) { float2 uTexel; float2 uSrcSize; };\n"
    "static const float4 dt = float4(1,1,1,1);\n"
    "float4 main(float4 pos : SV_Position, float2 uv : TEXCOORD0) : SV_Target {\n"
    "    float2 cTex = uv * uSrcSize * 1.00001;\n"
    "    #define TX(x,y) uTex.SampleLevel(uSamp,(floor(cTex+float2(x,y))+0.5)*uTexel,0.0)\n"
    "    float4 c00=TX(-0.25,-0.25), c20=TX(0.25,-0.25), c02=TX(-0.25,0.25), c22=TX(0.25,0.25);\n"
    "    float m1 = dot(abs(c00-c22), dt) + 0.001;\n"
    "    float m2 = dot(abs(c02-c20), dt) + 0.001;\n"
    "    return (m1*(c02+c20) + m2*(c22+c00)) / (2.0*(m1+m2));\n"
    "}\n";

// --- upscale: ScaleHQ 2x (DisciplesGL scalehq 2x) ---
const char* const kScaleHQ2Ps =
    "Texture2D uTex : register(t0);\n"
    "SamplerState uSamp : register(s0);\n"
    "cbuffer Cb : register(b0) { float2 uTexel; float2 uSrcSize; };\n"
    "static const float4 dt = float4(1,1,1,1);\n"
    "float4 main(float4 pos : SV_Position, float2 uv : TEXCOORD0) : SV_Target {\n"
    "    float MX=0.325, K=-0.250, MAXW=0.25, MINW=-0.05, LUMADD=0.25;\n"
    "    float4 c11 = uTex.SampleLevel(uSamp,(floor(uv*uSrcSize)+0.5)*uTexel,0.0);\n"
    "    float2 texel;\n"
    "    #define T2(x,y) uTex.SampleLevel(uSamp,(texel+float2(x,y))*uTexel,0.0)\n"
    "    texel = floor(uv*uSrcSize - 0.5) + 0.5;\n"
    "    float4 c00=T2(0,0),c02=T2(0,1),c20=T2(1,0),c22=T2(1,1);\n"
    "    texel = floor(uv*uSrcSize - float2(0.0,0.5)) + 0.5;\n"
    "    float4 c10=T2(0,0),c12=T2(0,1);\n"
    "    texel = floor(uv*uSrcSize - float2(0.5,0.0)) + 0.5;\n"
    "    float4 c01=T2(0,0),c21=T2(1,0);\n"
    "    float md1=dot(abs(c00-c22),dt), md2=dot(abs(c02-c20),dt);\n"
    "    float w1=dot(abs(c22-c11),dt)*md2, w2=dot(abs(c02-c11),dt)*md1;\n"
    "    float w3=dot(abs(c00-c11),dt)*md2, w4=dot(abs(c20-c11),dt)*md1;\n"
    "    float t1=w1+w3, t2=w2+w4; float ww=max(t1,t2)+0.0001;\n"
    "    c11=(w1*c00+w2*c20+w3*c22+w4*c02+ww*c11)/(t1+t2+ww);\n"
    "    float lc1=K/(0.12*dot(c10+c12+c11,dt)+LUMADD);\n"
    "    float lc2=K/(0.12*dot(c01+c21+c11,dt)+LUMADD);\n"
    "    w1=clamp(lc1*dot(abs(c11-c10),dt)+MX,MINW,MAXW);\n"
    "    w2=clamp(lc2*dot(abs(c11-c21),dt)+MX,MINW,MAXW);\n"
    "    w3=clamp(lc1*dot(abs(c11-c12),dt)+MX,MINW,MAXW);\n"
    "    w4=clamp(lc2*dot(abs(c11-c01),dt)+MX,MINW,MAXW);\n"
    "    return w1*c10+w2*c21+w3*c12+w4*c01+(1.0-w1-w2-w3-w4)*c11;\n"
    "}\n";

// --- upscale: xBRZ 2x (libretro 2xbrz, DisciplesGL xbrz 2x) ---
const char* const kXbrz2Ps =
    "Texture2D uTex : register(t0);\n"
    "SamplerState uSamp : register(s0);\n"
    "cbuffer Cb : register(b0) { float2 uTexel; float2 uSrcSize; };\n"
    "#define BLEND_NONE 0\n#define BLEND_NORMAL 1\n#define BLEND_DOMINANT 2\n"
    "#define LUMW 1.0\n#define EQTOL (30.0/255.0)\n#define STEEPT 2.2\n#define DOMT 3.6\n"
    "#define MPI 3.1415926535\n"
    "float reduce(float3 c){ return dot(c, float3(65536.0,256.0,1.0)); }\n"
    "float dist(float3 a, float3 b){\n"
    "  const float3 w=float3(0.2627,0.6780,0.0593);\n"
    "  float sB=0.5/(1.0-w.b), sR=0.5/(1.0-w.r);\n"
    "  float3 d=a-b; float Y=dot(d,w); float Cb=sB*(d.b-Y); float Cr=sR*(d.r-Y);\n"
    "  return sqrt((LUMW*Y)*(LUMW*Y)+Cb*Cb+Cr*Cr);\n"
    "}\n"
    "bool eq(float3 a, float3 b){ return dist(a,b)<EQTOL; }\n"
    "void ScalePixel(int4 blend, float3 k[9], inout float3 dst[4]){\n"
    "  float v0=reduce(k[0]),v4=reduce(k[4]),v5=reduce(k[5]),v7=reduce(k[7]),v8=reduce(k[8]);\n"
    "  float d14=dist(k[1],k[4]), d38=dist(k[3],k[8]);\n"
    "  bool shallow=(STEEPT*d14<=d38)&&(v0!=v4)&&(v5!=v4);\n"
    "  bool steep=(STEEPT*d38<=d14)&&(v0!=v8)&&(v7!=v8);\n"
    "  bool need=(blend[2]!=BLEND_NONE);\n"
    "  bool doLine=(blend[2]>=BLEND_DOMINANT || !((blend[1]!=BLEND_NONE && !eq(k[0],k[4])) || (blend[3]!=BLEND_NONE && !eq(k[0],k[8])) || (eq(k[4],k[3])&&eq(k[3],k[2])&&eq(k[2],k[1])&&eq(k[1],k[8])&&!eq(k[0],k[2]))));\n"
    "  float3 bp = (dist(k[0],k[1])<=dist(k[0],k[3])) ? k[1] : k[3];\n"
    "  dst[1]=lerp(dst[1],bp,(need&&doLine&&steep)?0.25:0.0);\n"
    "  dst[2]=lerp(dst[2],bp,(need)?((doLine)?((shallow)?((steep)?5.0/6.0:0.75):((steep)?0.75:0.5)):1.0-(MPI/4.0)):0.0);\n"
    "  dst[3]=lerp(dst[3],bp,(need&&doLine&&shallow)?0.25:0.0);\n"
    "}\n"
    "float4 main(float4 pos : SV_Position, float2 uv : TEXCOORD0) : SV_Target {\n"
    "  float2 ps = uTexel;\n"
    "  float2 p = (floor(uv*uSrcSize) + 0.5) * ps;\n"
    "  #define S(x,y) uTex.SampleLevel(uSamp, p+float2(x,y)*ps, 0.0).rgb\n"
    "  float3 s[25];\n"
    "  s[21]=S(-1,-2);s[22]=S(0,-2);s[23]=S(1,-2);\n"
    "  s[6]=S(-1,-1);s[7]=S(0,-1);s[8]=S(1,-1);\n"
    "  s[5]=S(-1,0);s[0]=S(0,0);s[1]=S(1,0);\n"
    "  s[4]=S(-1,1);s[3]=S(0,1);s[2]=S(1,1);\n"
    "  s[15]=S(-1,2);s[14]=S(0,2);s[13]=S(1,2);\n"
    "  s[19]=S(-2,-1);s[18]=S(-2,0);s[17]=S(-2,1);\n"
    "  s[9]=S(2,-1);s[10]=S(2,0);s[11]=S(2,1);\n"
    "  float v[9]; for(int n=0;n<9;n++) v[n]=reduce(s[n]);\n"
    "  int4 br = int4(0,0,0,0);\n"
    "  if(!((v[0]==v[1]&&v[3]==v[2])||(v[0]==v[3]&&v[1]==v[2]))){\n"
    "    float d1=dist(s[4],s[0])+dist(s[0],s[8])+dist(s[14],s[2])+dist(s[2],s[10])+4.0*dist(s[3],s[1]);\n"
    "    float d2=dist(s[5],s[3])+dist(s[3],s[13])+dist(s[7],s[1])+dist(s[1],s[11])+4.0*dist(s[0],s[2]);\n"
    "    bool dom=(DOMT*d1)<d2; br[2]=((d1<d2)&&(v[0]!=v[1])&&(v[0]!=v[3]))?(dom?BLEND_DOMINANT:BLEND_NORMAL):BLEND_NONE; }\n"
    "  if(!((v[5]==v[0]&&v[4]==v[3])||(v[5]==v[4]&&v[0]==v[3]))){\n"
    "    float d1=dist(s[17],s[5])+dist(s[5],s[7])+dist(s[15],s[3])+dist(s[3],s[1])+4.0*dist(s[4],s[0]);\n"
    "    float d2=dist(s[18],s[4])+dist(s[4],s[14])+dist(s[6],s[0])+dist(s[0],s[2])+4.0*dist(s[5],s[3]);\n"
    "    bool dom=(DOMT*d2)<d1; br[3]=((d2<d1)&&(v[0]!=v[5])&&(v[0]!=v[3]))?(dom?BLEND_DOMINANT:BLEND_NORMAL):BLEND_NONE; }\n"
    "  if(!((v[7]==v[8]&&v[0]==v[1])||(v[7]==v[0]&&v[8]==v[1]))){\n"
    "    float d1=dist(s[5],s[7])+dist(s[7],s[23])+dist(s[3],s[1])+dist(s[1],s[9])+4.0*dist(s[0],s[8]);\n"
    "    float d2=dist(s[6],s[0])+dist(s[0],s[2])+dist(s[22],s[8])+dist(s[8],s[10])+4.0*dist(s[7],s[1]);\n"
    "    bool dom=(DOMT*d2)<d1; br[1]=((d2<d1)&&(v[0]!=v[7])&&(v[0]!=v[1]))?(dom?BLEND_DOMINANT:BLEND_NORMAL):BLEND_NONE; }\n"
    "  if(!((v[6]==v[7]&&v[5]==v[0])||(v[6]==v[5]&&v[7]==v[0]))){\n"
    "    float d1=dist(s[18],s[6])+dist(s[6],s[22])+dist(s[4],s[0])+dist(s[0],s[8])+4.0*dist(s[5],s[7]);\n"
    "    float d2=dist(s[19],s[5])+dist(s[5],s[3])+dist(s[21],s[7])+dist(s[7],s[1])+4.0*dist(s[6],s[0]);\n"
    "    bool dom=(DOMT*d1)<d2; br[0]=((d1<d2)&&(v[0]!=v[5])&&(v[0]!=v[7]))?(dom?BLEND_DOMINANT:BLEND_NORMAL):BLEND_NONE; }\n"
    "  float3 dst[4]; dst[0]=s[0];dst[1]=s[0];dst[2]=s[0];dst[3]=s[0];\n"
    "  if(br[0]!=BLEND_NONE||br[1]!=BLEND_NONE||br[2]!=BLEND_NONE||br[3]!=BLEND_NONE){\n"
    "    float3 k[9]; float3 t3;\n"
    "    k[0]=s[0];k[1]=s[1];k[2]=s[2];k[3]=s[3];k[4]=s[4];k[5]=s[5];k[6]=s[6];k[7]=s[7];k[8]=s[8];\n"
    "    ScalePixel(br.xyzw,k,dst);\n"
    "    k[1]=s[7];k[2]=s[8];k[3]=s[1];k[4]=s[2];k[5]=s[3];k[6]=s[4];k[7]=s[5];k[8]=s[6];\n"
    "    t3=dst[3];dst[3]=dst[2];dst[2]=dst[1];dst[1]=dst[0];dst[0]=t3; ScalePixel(br.wxyz,k,dst);\n"
    "    k[1]=s[5];k[2]=s[6];k[3]=s[7];k[4]=s[8];k[5]=s[1];k[6]=s[2];k[7]=s[3];k[8]=s[4];\n"
    "    t3=dst[3];dst[3]=dst[2];dst[2]=dst[1];dst[1]=dst[0];dst[0]=t3; ScalePixel(br.zwxy,k,dst);\n"
    "    k[1]=s[3];k[2]=s[4];k[3]=s[5];k[4]=s[6];k[5]=s[7];k[6]=s[8];k[7]=s[1];k[8]=s[2];\n"
    "    t3=dst[3];dst[3]=dst[2];dst[2]=dst[1];dst[1]=dst[0];dst[0]=t3; ScalePixel(br.yzwx,k,dst);\n"
    "    t3=dst[3];dst[3]=dst[2];dst[2]=dst[1];dst[1]=dst[0];dst[0]=t3;\n"
    "  }\n"
    "  float2 fp = frac(uv * uSrcSize);\n"
    "  float3 res = fp.y<0.5 ? (fp.x<0.5?dst[0]:dst[1]) : (fp.x<0.5?dst[3]:dst[2]);\n"
    "  return float4(res, 1.0);\n"
    "}\n";

struct CbData {
    float fTexel[2];
    float fSrcSize[2];
};

struct FrameShader {
    bool fTried = false;
    bool fReady = false;
    ID3D11Device* fDevice = nullptr;
    ID3D11DeviceContext* fContext = nullptr;
    ID3D11VertexShader* fVs = nullptr;
    ID3D11Buffer* fCb = nullptr;
    ID3D11SamplerState* fSampPoint = nullptr;
    ID3D11SamplerState* fSampLinear = nullptr;
    ID3D11BlendState* fBlend = nullptr;
    ID3D11RasterizerState* fRaster = nullptr;
    ID3D11Texture2D* fSource = nullptr;
    ID3D11ShaderResourceView* fSourceSrv = nullptr;
    int fSourceW = 0;
    int fSourceH = 0;
    // compiled pixel shaders, by HLSL-source pointer
    std::map<const char*, ID3D11PixelShader*> fPs;
    // intermediate Nx render target (upscale stage)
    ID3D11Texture2D* fMid = nullptr;
    ID3D11RenderTargetView* fMidRtv = nullptr;
    ID3D11ShaderResourceView* fMidSrv = nullptr;
    int fMidW = 0;
    int fMidH = 0;
    ID3D11Texture2D* fMid2 = nullptr;
    ID3D11RenderTargetView* fMid2Rtv = nullptr;
    ID3D11ShaderResourceView* fMid2Srv = nullptr;
    int fMid2W = 0;
    int fMid2H = 0;
};

FrameShader gShader;
int gInterp = 3;   // cubic
int gUpscale = 0;  // none
int gFactor = 2;

ID3DBlob* compile(const char* const src, const char* const target) {
    ID3DBlob* code = nullptr;
    ID3DBlob* err = nullptr;
    const HRESULT hr = D3DCompile(src, std::strlen(src), nullptr, nullptr, nullptr,
                                  "main", target,
                                  D3DCOMPILE_OPTIMIZATION_LEVEL3, 0, &code, &err);
    if(FAILED(hr)) {
        if(err) {
            fprintf(stderr, "HLSL compile (%s) failed: %s\n", target,
                    static_cast<const char*>(err->GetBufferPointer()));
            err->Release();
        }
        if(code) code->Release();
        return nullptr;
    }
    if(err) err->Release();
    return code;
}

ID3D11PixelShader* getPs(FrameShader& s, const char* const src) {
    const auto it = s.fPs.find(src);
    if(it != s.fPs.end()) return it->second;
    ID3DBlob* blob = compile(src, "ps_5_0");
    if(!blob) blob = compile(src, "ps_4_0");
    ID3D11PixelShader* ps = nullptr;
    if(blob) {
        const HRESULT hr = s.fDevice->CreatePixelShader(blob->GetBufferPointer(),
                                                        blob->GetBufferSize(),
                                                        nullptr, &ps);
        if(FAILED(hr)) {
            printf("post-process: CreatePixelShader failed hr=0x%08lx\n",
                   static_cast<unsigned long>(hr));
        }
        blob->Release();
    }
    s.fPs[src] = ps; // cache even null to avoid recompiling a broken shader
    return ps;
}

bool initShader(SDL_Renderer* const r) {
    auto& s = gShader;
    if(s.fTried) return s.fReady;
    s.fTried = true;

    s.fDevice = SDL_RenderGetD3D11Device(r);
    if(!s.fDevice) {
        printf("post-process: not a D3D11 renderer\n");
        return false;
    }
    s.fDevice->GetImmediateContext(&s.fContext);
    if(!s.fContext) return false;

    ID3DBlob* vs = compile(kVs, "vs_4_0");
    if(!vs) return false;
    HRESULT hr = s.fDevice->CreateVertexShader(vs->GetBufferPointer(),
                                               vs->GetBufferSize(), nullptr, &s.fVs);
    vs->Release();
    if(FAILED(hr)) {
        printf("post-process: CreateVertexShader failed hr=0x%08lx\n",
               static_cast<unsigned long>(hr));
        return false;
    }

    D3D11_BUFFER_DESC cbd{};
    cbd.ByteWidth = sizeof(CbData);
    cbd.Usage = D3D11_USAGE_DYNAMIC;
    cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    hr = s.fDevice->CreateBuffer(&cbd, nullptr, &s.fCb);
    if(FAILED(hr)) {
        printf("post-process: CreateBuffer cb failed hr=0x%08lx\n",
               static_cast<unsigned long>(hr));
        return false;
    }

    D3D11_SAMPLER_DESC sd{};
    sd.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
    sd.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
    sd.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
    sd.ComparisonFunc = D3D11_COMPARISON_NEVER;
    sd.MaxLOD = D3D11_FLOAT32_MAX;
    sd.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
    hr = s.fDevice->CreateSamplerState(&sd, &s.fSampPoint);
    if(FAILED(hr)) {
        printf("post-process: CreateSampler point failed hr=0x%08lx\n",
               static_cast<unsigned long>(hr));
        return false;
    }
    sd.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    hr = s.fDevice->CreateSamplerState(&sd, &s.fSampLinear);
    if(FAILED(hr)) {
        printf("post-process: CreateSampler linear failed hr=0x%08lx\n",
               static_cast<unsigned long>(hr));
        return false;
    }

    D3D11_BLEND_DESC bd{};
    bd.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
    hr = s.fDevice->CreateBlendState(&bd, &s.fBlend);
    if(FAILED(hr)) {
        printf("post-process: CreateBlendState failed hr=0x%08lx\n",
               static_cast<unsigned long>(hr));
        return false;
    }

    D3D11_RASTERIZER_DESC rd{};
    rd.FillMode = D3D11_FILL_SOLID;
    rd.CullMode = D3D11_CULL_NONE;
    rd.DepthClipEnable = TRUE;
    hr = s.fDevice->CreateRasterizerState(&rd, &s.fRaster);
    if(FAILED(hr)) {
        printf("post-process: CreateRasterizerState failed hr=0x%08lx\n",
               static_cast<unsigned long>(hr));
        return false;
    }

    s.fReady = true;
    return true;
}

void releaseRenderTarget(ID3D11Texture2D*& tex,
                         ID3D11RenderTargetView*& rtv,
                         ID3D11ShaderResourceView*& srv,
                         int& w,
                         int& h) {
    if(srv) { srv->Release(); srv = nullptr; }
    if(rtv) { rtv->Release(); rtv = nullptr; }
    if(tex) { tex->Release(); tex = nullptr; }
    w = 0;
    h = 0;
}

bool ensureRenderTarget(FrameShader& s,
                        ID3D11Texture2D*& tex,
                        ID3D11RenderTargetView*& rtv,
                        ID3D11ShaderResourceView*& srv,
                        int& currentW,
                        int& currentH,
                        const int w,
                        const int h) {
    if(tex && currentW == w && currentH == h) return true;
    releaseRenderTarget(tex, rtv, srv, currentW, currentH);
    if(w <= 0 || h <= 0 || w > 8192 || h > 8192) return false;

    D3D11_TEXTURE2D_DESC td{};
    td.Width = w;
    td.Height = h;
    td.MipLevels = 1;
    td.ArraySize = 1;
    td.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    td.SampleDesc.Count = 1;
    td.Usage = D3D11_USAGE_DEFAULT;
    td.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
    if(FAILED(s.fDevice->CreateTexture2D(&td, nullptr, &tex))) return false;
    if(FAILED(s.fDevice->CreateRenderTargetView(tex, nullptr, &rtv))) {
        releaseRenderTarget(tex, rtv, srv, currentW, currentH);
        return false;
    }
    if(FAILED(s.fDevice->CreateShaderResourceView(tex, nullptr, &srv))) {
        releaseRenderTarget(tex, rtv, srv, currentW, currentH);
        return false;
    }
    currentW = w;
    currentH = h;
    return true;
}

bool ensureMid(FrameShader& s, const int w, const int h) {
    return ensureRenderTarget(s, s.fMid, s.fMidRtv, s.fMidSrv,
                              s.fMidW, s.fMidH, w, h);
}

bool ensureMid2(FrameShader& s, const int w, const int h) {
    return ensureRenderTarget(s, s.fMid2, s.fMid2Rtv, s.fMid2Srv,
                              s.fMid2W, s.fMid2H, w, h);
}

bool ensureSource(FrameShader& s, const int w, const int h) {
    if(s.fSource && s.fSourceW == w && s.fSourceH == h) return true;
    if(s.fSourceSrv) { s.fSourceSrv->Release(); s.fSourceSrv = nullptr; }
    if(s.fSource) { s.fSource->Release(); s.fSource = nullptr; }
    s.fSourceW = 0;
    s.fSourceH = 0;
    if(w <= 0 || h <= 0 || w > 8192 || h > 8192) return false;

    D3D11_TEXTURE2D_DESC td{};
    td.Width = w;
    td.Height = h;
    td.MipLevels = 1;
    td.ArraySize = 1;
    td.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    td.SampleDesc.Count = 1;
    td.Usage = D3D11_USAGE_DEFAULT;
    td.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    const HRESULT texHr = s.fDevice->CreateTexture2D(&td, nullptr, &s.fSource);
    if(FAILED(texHr)) {
        printf("post-process: Create source texture failed hr=0x%08lx\n",
               static_cast<unsigned long>(texHr));
        return false;
    }
    const HRESULT srvHr = s.fDevice->CreateShaderResourceView(
        s.fSource, nullptr, &s.fSourceSrv);
    if(FAILED(srvHr)) {
        printf("post-process: Create source SRV failed hr=0x%08lx\n",
               static_cast<unsigned long>(srvHr));
        s.fSource->Release();
        s.fSource = nullptr;
        return false;
    }
    s.fSourceW = w;
    s.fSourceH = h;
    return true;
}

void setCb(FrameShader& s, const int srcW, const int srcH) {
    D3D11_MAPPED_SUBRESOURCE map{};
    if(SUCCEEDED(s.fContext->Map(s.fCb, 0, D3D11_MAP_WRITE_DISCARD, 0, &map))) {
        CbData cb;
        cb.fTexel[0] = 1.f / float(srcW);
        cb.fTexel[1] = 1.f / float(srcH);
        cb.fSrcSize[0] = float(srcW);
        cb.fSrcSize[1] = float(srcH);
        std::memcpy(map.pData, &cb, sizeof(cb));
        s.fContext->Unmap(s.fCb, 0);
    }
}

// Pick the upscale PS + its native factor. 4x is done by chaining 2x passes.
const char* upscalePs(const int upscale, const int requestedFactor,
                      int& nativeFactor) {
    switch(upscale) {
    case 1: // xbrz 2x core
        nativeFactor = 2; return kXbrz2Ps;
    case 2: // scalehq 2x
        nativeFactor = 2; return kScaleHQ2Ps;
    case 3: // scalenx 2x or 3x
        if(requestedFactor == 3) { nativeFactor = 3; return kScaleNx3Ps; }
        nativeFactor = 2; return kScaleNx2Ps;
    case 4: // eagle 2x
        nativeFactor = 2; return kEaglePs;
    case 5: // xsal 2x
        nativeFactor = 2; return kXSalPs;
    default:
        nativeFactor = 1;
        return nullptr;
    }
}

const char* interpPs(const int interp, bool& linear) {
    switch(interp) {
    case 0: linear = false; return kBlitPs;   // nearest
    case 1: linear = true;  return kBlitPs;   // linear
    case 2: linear = true;  return kHermitePs;
    case 4: linear = true;  return kLanczosPs;
    case 3:
    default: linear = true; return kCubicPs;  // cubic
    }
}

const char* const kProbePs =
    "float4 main(float4 pos : SV_Position, float2 uv : TEXCOORD0) : SV_Target {\n"
    "    return float4(1.0, 0.0, 1.0, 1.0);\n"
    "}\n";

}

void setPostprocessFilters(const int interpolation, const int upscale,
                           const int factor) {
    gInterp = interpolation;
    gUpscale = upscale;
    gFactor = factor < 2 ? 2 : (factor > 6 ? 6 : factor);
}

bool applySrvPostprocess(SDL_Renderer* const r,
                         ID3D11ShaderResourceView* const frameSrv,
                         const int texW,
                         const int texH,
                         const SDL_Rect* const dstRect,
                         const bool outputToBackbuffer,
                         const bool applyUpscale) {
    if(!initShader(r)) {
        return false;
    }
    auto& s = gShader;

    if(!frameSrv || texW <= 0 || texH <= 0) return false;

    if(outputToBackbuffer) SDL_SetRenderTarget(r, nullptr);
    SDL_RenderFlush(r);
    auto* const ctx = s.fContext;

    ID3D11RenderTargetView* oldRtvs[D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT] = {};
    ID3D11DepthStencilView* oldDsv = nullptr;
    ctx->OMGetRenderTargets(D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT, oldRtvs, &oldDsv);
    ID3D11RasterizerState* oldRaster = nullptr;
    ctx->RSGetState(&oldRaster);
    D3D11_VIEWPORT oldViewports[D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE] = {};
    UINT oldViewportCount = D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE;
    ctx->RSGetViewports(&oldViewportCount, oldViewports);
    ID3D11BlendState* oldBlend = nullptr;
    FLOAT oldBlendFactor[4] = {};
    UINT oldSampleMask = 0;
    ctx->OMGetBlendState(&oldBlend, oldBlendFactor, &oldSampleMask);
    ID3D11InputLayout* oldInputLayout = nullptr;
    ctx->IAGetInputLayout(&oldInputLayout);
    D3D11_PRIMITIVE_TOPOLOGY oldTopology = D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED;
    ctx->IAGetPrimitiveTopology(&oldTopology);
    ID3D11VertexShader* oldVs = nullptr;
    ID3D11ClassInstance* oldVsClasses[16] = {};
    UINT oldVsClassCount = 16;
    ctx->VSGetShader(&oldVs, oldVsClasses, &oldVsClassCount);
    ID3D11PixelShader* oldPs = nullptr;
    ID3D11ClassInstance* oldPsClasses[16] = {};
    UINT oldPsClassCount = 16;
    ctx->PSGetShader(&oldPs, oldPsClasses, &oldPsClassCount);
    ID3D11ShaderResourceView* oldPsSrv0 = nullptr;
    ctx->PSGetShaderResources(0, 1, &oldPsSrv0);
    ID3D11SamplerState* oldPsSamp0 = nullptr;
    ctx->PSGetSamplers(0, 1, &oldPsSamp0);
    ID3D11Buffer* oldPsCb0 = nullptr;
    ctx->PSGetConstantBuffers(0, 1, &oldPsCb0);

    const auto restoreState = [&]() {
        ctx->PSSetShaderResources(0, 1, &oldPsSrv0);
        ctx->PSSetSamplers(0, 1, &oldPsSamp0);
        ctx->PSSetConstantBuffers(0, 1, &oldPsCb0);
        ctx->VSSetShader(oldVs, oldVsClasses, oldVsClassCount);
        ctx->PSSetShader(oldPs, oldPsClasses, oldPsClassCount);
        ctx->IASetInputLayout(oldInputLayout);
        ctx->IASetPrimitiveTopology(oldTopology);
        if(oldViewportCount > 0) ctx->RSSetViewports(oldViewportCount, oldViewports);
        ctx->RSSetState(oldRaster);
        ctx->OMSetBlendState(oldBlend, oldBlendFactor, oldSampleMask);
        ctx->OMSetRenderTargets(D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT, oldRtvs, oldDsv);
        for(auto* v : oldRtvs) if(v) v->Release();
        if(oldDsv) oldDsv->Release();
        if(oldRaster) oldRaster->Release();
        if(oldBlend) oldBlend->Release();
        if(oldInputLayout) oldInputLayout->Release();
        if(oldVs) oldVs->Release();
        for(UINT i = 0; i < oldVsClassCount; i++) if(oldVsClasses[i]) oldVsClasses[i]->Release();
        if(oldPs) oldPs->Release();
        for(UINT i = 0; i < oldPsClassCount; i++) if(oldPsClasses[i]) oldPsClasses[i]->Release();
        if(oldPsSrv0) oldPsSrv0->Release();
        if(oldPsSamp0) oldPsSamp0->Release();
        if(oldPsCb0) oldPsCb0->Release();
    };

    void* rendererData = nullptr;
    ID3D11RenderTargetView* const backbufferRtv = mainRtv(r, &rendererData);
    ID3D11RenderTargetView* const rtv = outputToBackbuffer ?
        backbufferRtv : oldRtvs[0];
    if(!rtv) { restoreState(); return false; }

    int outW = 0;
    int outH = 0;
    if(dstRect) {
        outW = dstRect->w;
        outH = dstRect->h;
    } else {
        SDL_GetRendererOutputSize(r, &outW, &outH);
    }
    if(outW <= 0 || outH <= 0) { restoreState(); return false; }

    const float blendFactor[4] = {0, 0, 0, 0};
    ID3D11ShaderResourceView* const nullSrv = nullptr;

    ctx->RSSetState(s.fRaster);
    ctx->OMSetBlendState(s.fBlend, blendFactor, 0xffffffff);
    ctx->IASetInputLayout(nullptr);
    ctx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    ctx->GSSetShader(nullptr, nullptr, 0);
    ctx->HSSetShader(nullptr, nullptr, 0);
    ctx->DSSetShader(nullptr, nullptr, 0);
    ctx->VSSetShader(s.fVs, nullptr, 0);
    ctx->PSSetConstantBuffers(0, 1, &s.fCb);

    // --- stage 1: optional upscale (frame -> Nx intermediate) ---
    ID3D11ShaderResourceView* interpSrc = frameSrv;
    int interpW = texW;
    int interpH = texH;
    int factor = gFactor;
    int nativeFactor = 1;
    const char* upSrc = applyUpscale ?
        upscalePs(gUpscale, factor, nativeFactor) : nullptr;
    if(upSrc) {
        ID3D11PixelShader* const upPs = getPs(s, upSrc);
        const int midW = texW * nativeFactor;
        const int midH = texH * nativeFactor;
        if(upPs && ensureMid(s, midW, midH)) {
            setCb(s, texW, texH);
            D3D11_VIEWPORT vp{};
            vp.Width = float(midW);
            vp.Height = float(midH);
            vp.MaxDepth = 1.f;
            ctx->RSSetViewports(1, &vp);
            ctx->OMSetRenderTargets(1, &s.fMidRtv, nullptr);
            ctx->PSSetShader(upPs, nullptr, 0);
            ctx->PSSetSamplers(0, 1, &s.fSampPoint); // pixel-art = point
            ctx->PSSetShaderResources(0, 1, &frameSrv);
            ctx->Draw(3, 0);
            ctx->PSSetShaderResources(0, 1, &nullSrv);
            interpSrc = s.fMidSrv;
            interpW = midW;
            interpH = midH;

            if(factor >= 4 && nativeFactor == 2) {
                const int mid2W = texW * 4;
                const int mid2H = texH * 4;
                if(ensureMid2(s, mid2W, mid2H)) {
                    setCb(s, midW, midH);
                    D3D11_VIEWPORT vp2{};
                    vp2.Width = float(mid2W);
                    vp2.Height = float(mid2H);
                    vp2.MaxDepth = 1.f;
                    ctx->RSSetViewports(1, &vp2);
                    ctx->OMSetRenderTargets(1, &s.fMid2Rtv, nullptr);
                    ctx->PSSetShader(upPs, nullptr, 0);
                    ctx->PSSetSamplers(0, 1, &s.fSampPoint);
                    ctx->PSSetShaderResources(0, 1, &s.fMidSrv);
                    ctx->Draw(3, 0);
                    ctx->PSSetShaderResources(0, 1, &nullSrv);
                    interpSrc = s.fMid2Srv;
                    interpW = mid2W;
                    interpH = mid2H;
                    factor = 4;
                }
            } else {
                factor = nativeFactor;
            }
        }
    }

    // --- stage 2: interpolation resample (-> output target) ---
    const float srcAspect = float(texW) / float(texH);
    const float outAspect = float(outW) / float(outH);
    float vpW = float(outW);
    float vpH = float(outH);
    if(outAspect > srcAspect) vpW = vpH * srcAspect; else vpH = vpW / srcAspect;
    float vpX = (float(outW) - vpW) * 0.5f;
    float vpY = (float(outH) - vpH) * 0.5f;
    if(dstRect) {
        vpX += float(dstRect->x);
        vpY += float(dstRect->y);
    }

    const float black[4] = {0, 0, 0, 1};
    if(outputToBackbuffer) ctx->ClearRenderTargetView(rtv, black);

    bool linear = true;
    const char* inSrc = interpPs(gInterp, linear);
    ID3D11PixelShader* const inPs = getPs(s, inSrc);
    if(!inPs) {
        static bool once = false;
        if(!once) { once = true; printf("post-process: interp PS compile failed (interp=%d)\n", gInterp); }
        restoreState(); return false;
    }

    setCb(s, interpW, interpH);
    D3D11_VIEWPORT vp{};
    vp.TopLeftX = vpX;
    vp.TopLeftY = vpY;
    vp.Width = vpW;
    vp.Height = vpH;
    vp.MaxDepth = 1.f;
    ctx->RSSetViewports(1, &vp);
    ctx->OMSetRenderTargets(1, &rtv, nullptr);
    ctx->PSSetShader(inPs, nullptr, 0);
    ID3D11SamplerState* const samp = linear ? s.fSampLinear : s.fSampPoint;
    ctx->PSSetSamplers(0, 1, &samp);
    ctx->PSSetShaderResources(0, 1, &interpSrc);
    ctx->Draw(3, 0);
    ID3D11ShaderResourceView* nullSrvs[8] = {};
    ctx->PSSetShaderResources(0, 8, nullSrvs);
    ctx->VSSetShaderResources(0, 8, nullSrvs);
    ctx->OMSetRenderTargets(0, nullptr, nullptr);
    ctx->Flush();
    restoreState();
    dirtySdlD3D11State(rendererData);

    return true;
}

bool applyPostprocess(SDL_Renderer* const r,
                      const void* const pixels,
                      const int pitch,
                      const int texW,
                      const int texH,
                      const SDL_Rect* const dstRect,
                      const bool outputToBackbuffer,
                      const bool applyUpscale) {
    if(!initShader(r)) return false;
    auto& s = gShader;

    if(!pixels || pitch <= 0 || texW <= 0 || texH <= 0) return false;
    if(!ensureSource(s, texW, texH)) return false;
    s.fContext->UpdateSubresource(s.fSource, 0, nullptr, pixels, pitch, 0);
    return applySrvPostprocess(r, s.fSourceSrv, texW, texH, dstRect,
                               outputToBackbuffer, applyUpscale);
}

bool applyTexturePostprocess(SDL_Renderer* const r,
                             SDL_Texture* const texture,
                             const int textureW,
                             const int textureH,
                             const bool applyUpscale) {
    return applySrvPostprocess(r, textureSrv(texture), textureW, textureH,
                               nullptr, true, applyUpscale);
}

bool applyFullFramePostprocess(SDL_Renderer* const r,
                               const void* const pixels,
                               const int pitch,
                               const int texW,
                               const int texH,
                               const bool applyUpscale) {
    return applyPostprocess(r, pixels, pitch, texW, texH, nullptr, true,
                            applyUpscale);
}

#else
namespace {
int gInterp = 3;
int gUpscale = 0;
int gFactor = 2;
}

void setPostprocessFilters(const int interpolation, const int upscale,
                           const int factor) {
    gInterp = interpolation;
    gUpscale = upscale;
    gFactor = factor < 2 ? 2 : (factor > 6 ? 6 : factor);
}

bool applyFullFramePostprocess(SDL_Renderer* const r,
                               const void* const pixels,
                               const int pitch,
                               const int texW,
                               const int texH,
                               const bool applyUpscale) {
    (void)r;
    (void)pixels;
    (void)pitch;
    (void)texW;
    (void)texH;
    (void)applyUpscale;
    static bool once = false;
    if(!once) {
        once = true;
        printf("post-process: OpenGL GLSL backend not initialized yet "
               "(interp=%d upscale=%d factor=%d)\n",
               gInterp, gUpscale, gFactor);
    }
    return false;
}

bool applyTexturePostprocess(SDL_Renderer* const r,
                             SDL_Texture* const texture,
                             const int textureW,
                             const int textureH,
                             const bool applyUpscale) {
    (void)r;
    (void)texture;
    (void)textureW;
    (void)textureH;
    (void)applyUpscale;
    return false;
}

#endif
