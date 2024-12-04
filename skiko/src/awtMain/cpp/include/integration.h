#pragma once

#ifdef SK_DIRECT3D
#include <locale>
#include <Windows.h>
#include <jawt_md.h>
#include "jni_helpers.h"
#include "exceptions_handler.h"
#include "window_util.h"

#include "SkColorSpace.h"
#include "GrBackendSurface.h"
#include "GrDirectContext.h"
#include "SkSurface.h"
#include "include/gpu/ganesh/SkSurfaceGanesh.h"
#include "../common/interop.hh"
#include "DCompLibrary.h"

#include "d3d/GrD3DTypes.h"
#include <d3d12sdklayers.h>
#include "d3d/GrD3DBackendContext.h"
#include <d3d12.h>
#include <dxgi1_4.h>
#include <dxgi1_6.h>

void register_skia_ctx(GrD3DBackendContext backendCtx, sk_sp<GrDirectContext>& ctx);

extern "C" JNIEXPORT jlong JNICALL org_jetbrains_skia_Shader__1nMakeBackendRT(
            JNIEnv *env, jint width, jint height, jint pixel_format);

extern "C" JNIEXPORT jlong JNICALL Java_org_jetbrains_skia_ShaderKt__1nMakeBackendRT
  (JNIEnv* env, jint width, jint height, jint pixel_format);

#endif
