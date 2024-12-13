#include "integration.h"
#include <cassert>
#include <semaphore>

#include <include/core/SkTileMode.h>
#include <include/core/SkBitmap.h>
#include <include/core/SkPaint.h>
#include <include/core/SkTypeface.h>
#include <include/core/SkFont.h>
#include <include/core/SkMaskFilter.h>
#include <include/core/SkShader.h>
#include <include/codec/SkCodec.h>
#include <include/gpu/ganesh/SkImageGanesh.h>
//#include "src/gpu/ganesh/GrDirectContextPriv.h"
#include <wrl.h>
using namespace Microsoft::WRL;

static GrD3DBackendContext g_backendCtx;
static sk_sp<GrDirectContext> g_ctx;

void register_skia_ctx(GrD3DBackendContext backendCtx, sk_sp<GrDirectContext>& ctx)
{
    assert(!g_ctx);
    g_ctx = ctx;
    g_backendCtx = backendCtx;
}

struct RtShader {
    sk_sp<SkImage> image;
    sk_sp<SkShader> shader;
};
struct DoubleBufferRtShader {
    int index;
    RtShader rt_shader[2];
};

static RtShader create_texture(int width, int height, int pixel_format)
{
    // render  to texture
    D3D12_HEAP_PROPERTIES heapProperties;
    heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;
    heapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
    heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
    heapProperties.CreationNodeMask = 1;
    heapProperties.VisibleNodeMask = 1;

    D3D12_RESOURCE_DESC textureResourceDesc;
    textureResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    textureResourceDesc.Alignment = 0;
    textureResourceDesc.Width = width,                                  // Width
    textureResourceDesc.Height = height,                                // Height
    textureResourceDesc.DepthOrArraySize = 1,                           // Array size (1 texture)
    textureResourceDesc.MipLevels = 1,                                  // Mip levels
    textureResourceDesc.Format = (DXGI_FORMAT)pixel_format,             // Format
    textureResourceDesc.SampleDesc.Count = 1,                           // Sample count
    textureResourceDesc.SampleDesc.Quality = 0,                         // Sample quality
    textureResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
    textureResourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;// Enable render target usage

    D3D12_CLEAR_VALUE clearValue;
    clearValue.Format = (DXGI_FORMAT)pixel_format;
    clearValue.Color[0] = 0.0f;
    clearValue.Color[1] = 0.0f;
    clearValue.Color[2] = 0.0f;
    clearValue.Color[3] = 1.0f;  // Clear to opaque black

    // Create the texture resource
    ComPtr<ID3D12Resource> tex_renderTargetTexture;
    ComPtr<ID3D12DescriptorHeap> tex_rtvHeap;

    g_backendCtx.fDevice->CreateCommittedResource(
        &heapProperties,
        D3D12_HEAP_FLAG_NONE,
        &textureResourceDesc,
        D3D12_RESOURCE_STATE_RENDER_TARGET, // Initial state for a render target
        &clearValue,                        // Optional clear value
        IID_PPV_ARGS(&tex_renderTargetTexture)
    );

    // Create a descriptor heap for the RTV
    D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
    rtvHeapDesc.NumDescriptors = 1; // One RTV
    rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

    g_backendCtx.fDevice->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&tex_rtvHeap));

    // Create the RTV and store it in the descriptor heap
    D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle(tex_rtvHeap->GetCPUDescriptorHandleForHeapStart());
    g_backendCtx.fDevice->CreateRenderTargetView(tex_renderTargetTexture.Get(), nullptr, rtvHandle);

    GrD3DTextureResourceInfo info(nullptr,
        nullptr,
        D3D12_RESOURCE_STATE_PRESENT,
        (DXGI_FORMAT)pixel_format,
        1,
        1,
        0);
    info.fResource.retain(tex_renderTargetTexture.Get());
    GrBackendTexture backendTexture = GrBackendTexture(width, height, info);
    sk_sp<SkImage> skImage = SkImages::BorrowTextureFrom(
        g_ctx.get(),
        backendTexture,
        kTopLeft_GrSurfaceOrigin,
        kRGBA_8888_SkColorType,
        kPremul_SkAlphaType,
        nullptr
    );
    sk_sp<SkShader> skShader = skImage->makeRawShader(
        SkTileMode::kClamp,
        SkTileMode::kClamp,
        SkSamplingOptions());
    return RtShader{ skImage, skShader };
}

extern "C" JNIEXPORT jlong JNICALL org_jetbrains_skia_Shader_makeBackendRT(
            JNIEnv *env, jint width, jint height, jint pixel_format)
{
    DoubleBufferRtShader* d = new DoubleBufferRtShader{-1, {
        create_texture(width, height, pixel_format),
        create_texture(width, height, pixel_format)
    } };
    return toJavaPointer(d);
}

extern "C" JNIEXPORT jlong JNICALL Java_org_jetbrains_skia_ShaderKt_makeBackendRT
  (JNIEnv* env, jint width, jint height, jint pixel_format)
{
    return org_jetbrains_skia_Shader_makeBackendRT(env, width, height, pixel_format);
}

extern "C" JNIEXPORT jlong JNICALL org_jetbrains_skia_Shader_openShader(
            JNIEnv *env, jlong rtPtr)
{
    __try {
        DoubleBufferRtShader *rt = fromJavaPointer<DoubleBufferRtShader *>(rtPtr);

        rt->index = (rt->index + 1) % 2;
        RtShader * s = &rt->rt_shader[rt->index];

        // todo : switch to STATE_SHADER_RESOURCE

        return toJavaPointer(s->shader.get());
    }
    __except(EXCEPTION_EXECUTE_HANDLER) {
        auto code = GetExceptionCode();
        throwJavaRenderExceptionByExceptionCode(env, __FUNCTION__, code);
    }
}
extern "C" JNIEXPORT jlong JNICALL Java_org_jetbrains_skia_ShaderKt_openShader
  (JNIEnv* env, jlong rt)
{
    return org_jetbrains_skia_Shader_openShader(env, rt);
}

// transit res to shader_resource
void eldra_before_render()
{
}
// transit res to draw
void eldra_after_render()
{
    // todo : switch back to render target, release lock
}