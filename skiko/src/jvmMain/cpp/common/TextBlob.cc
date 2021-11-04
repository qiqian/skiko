#include <cstring>
#include <iostream>
#include <jni.h>
#include "SkData.h"
#include "SkSerialProcs.h"
#include "SkTextBlob.h"
#include "interop.hh"
#include "RunRecordClone.hh"

static void unrefTextBlob(SkTextBlob* ptr) {
    ptr->unref();
}

extern "C" JNIEXPORT jlong JNICALL Java_org_jetbrains_skia_TextBlobKt_TextBlob_1nGetFinalizer
  (JNIEnv* env, jclass jclass) {
    return static_cast<jlong>(reinterpret_cast<uintptr_t>(&unrefTextBlob));
}

extern "C" JNIEXPORT jobject JNICALL Java_org_jetbrains_skia_TextBlobKt__1nBounds
  (JNIEnv* env, jclass jclass, jlong ptr) {
    SkTextBlob* instance = reinterpret_cast<SkTextBlob*>(static_cast<uintptr_t>(ptr));
    SkRect bounds = instance->bounds();
    return skija::Rect::fromSkRect(env, instance->bounds());
}

extern "C" JNIEXPORT jint JNICALL Java_org_jetbrains_skia_TextBlobKt_TextBlob_1nGetUniqueId
  (JNIEnv* env, jclass jclass, jlong ptr) {
    SkTextBlob* instance = reinterpret_cast<SkTextBlob*>(static_cast<uintptr_t>(ptr));
    return instance->uniqueID();
}

extern "C" JNIEXPORT jfloatArray JNICALL Java_org_jetbrains_skia_TextBlobKt__1nGetIntercepts
  (JNIEnv* env, jclass jclass, jlong ptr, jfloat lower, jfloat upper, jlong paintPtr) {
    SkTextBlob* instance = reinterpret_cast<SkTextBlob*>(static_cast<uintptr_t>(ptr));
    std::vector<float> bounds {lower, upper};
    SkPaint* paint = reinterpret_cast<SkPaint*>(static_cast<uintptr_t>(paintPtr));
    int len = instance->getIntercepts(bounds.data(), nullptr, paint);
    std::vector<float> intervals(len);
    instance->getIntercepts(bounds.data(), intervals.data(), paint);
    return javaFloatArray(env, intervals);
}

extern "C" JNIEXPORT jlong JNICALL Java_org_jetbrains_skia_TextBlobKt__1nMakeFromPosH
  (JNIEnv* env, jclass jclass, jshortArray glyphsArr, jfloatArray xposArr, jfloat ypos, jlong fontPtr) {
    jsize len = env->GetArrayLength(glyphsArr);
    jshort* glyphs = env->GetShortArrayElements(glyphsArr, nullptr);
    jfloat* xpos = env->GetFloatArrayElements(xposArr, nullptr);
    SkFont* font = reinterpret_cast<SkFont*>(static_cast<uintptr_t>(fontPtr));

    SkTextBlob* instance = SkTextBlob::MakeFromPosTextH(glyphs, len * sizeof(jshort), xpos, ypos, *font, SkTextEncoding::kGlyphID).release();

    env->ReleaseShortArrayElements(glyphsArr, glyphs, 0);
    env->ReleaseFloatArrayElements(xposArr, xpos, 0);

    return reinterpret_cast<jlong>(instance);
}

extern "C" JNIEXPORT jlong JNICALL Java_org_jetbrains_skia_TextBlobKt__1nMakeFromPos
  (JNIEnv* env, jclass jclass, jshortArray glyphsArr, jint glyphsLen, jfloatArray posArr, jlong fontPtr ) {
    jshort* glyphs = env->GetShortArrayElements(glyphsArr, nullptr);
    jfloat* pos = env->GetFloatArrayElements(posArr, nullptr);
    SkFont* font = reinterpret_cast<SkFont*>(static_cast<uintptr_t>(fontPtr));

    SkTextBlob* instance = SkTextBlob::MakeFromPosText(
        glyphs,
        glyphsLen * sizeof(jshort),
        reinterpret_cast<SkPoint*>(pos),
        *font,
        SkTextEncoding::kGlyphID
    ).release();

    env->ReleaseShortArrayElements(glyphsArr, glyphs, 0);
    env->ReleaseFloatArrayElements(posArr, pos, 0);

    return reinterpret_cast<jlong>(instance);
}

extern "C" JNIEXPORT jlong JNICALL Java_org_jetbrains_skia_TextBlobKt__1nMakeFromRSXform
  (JNIEnv* env, jclass jclass, jshortArray glyphsArr, jfloatArray xformArr, jlong fontPtr ) {
    jsize len = env->GetArrayLength(glyphsArr);
    jshort* glyphs = env->GetShortArrayElements(glyphsArr, nullptr);
    jfloat* xform = env->GetFloatArrayElements(xformArr, nullptr);
    SkFont* font = reinterpret_cast<SkFont*>(static_cast<uintptr_t>(fontPtr));

    SkTextBlob* instance = SkTextBlob::MakeFromRSXform(glyphs, len * sizeof(jshort), reinterpret_cast<SkRSXform*>(xform), *font, SkTextEncoding::kGlyphID).release();

    env->ReleaseShortArrayElements(glyphsArr, glyphs, 0);
    env->ReleaseFloatArrayElements(xformArr, xform, 0);

    return reinterpret_cast<jlong>(instance);
}

extern "C" JNIEXPORT jlong JNICALL Java_org_jetbrains_skia_TextBlobKt_TextBlob_1nSerializeToData
  (JNIEnv* env, jclass jclass, jlong ptr) {
    SkTextBlob* instance = reinterpret_cast<SkTextBlob*>(static_cast<uintptr_t>(ptr));
    SkData* data = instance->serialize({}).release();
    return reinterpret_cast<jlong>(data);
}

extern "C" JNIEXPORT jlong JNICALL Java_org_jetbrains_skia_TextBlobKt_TextBlob_1nMakeFromData
  (JNIEnv* env, jclass jclass, jlong dataPtr) {
    SkData* data = reinterpret_cast<SkData*>(static_cast<uintptr_t>(dataPtr));
    SkTextBlob* instance = SkTextBlob::Deserialize(data->data(), data->size(), {}).release();
    return reinterpret_cast<jlong>(instance);
}

extern "C" JNIEXPORT jint JNICALL Java_org_jetbrains_skia_TextBlobKt__1nGetGlyphsLength
  (JNIEnv* env, jclass jclass, jlong ptr) {

  SkTextBlob* instance = reinterpret_cast<SkTextBlob*>(static_cast<uintptr_t>(ptr));
  SkTextBlob::Iter iter(*instance);
  SkTextBlob::Iter::Run run;
  jint stored = 0;
  while (iter.next(&run)) {
      stored += run.fGlyphCount;
  }
  return stored;
}

extern "C" JNIEXPORT void JNICALL Java_org_jetbrains_skia_TextBlobKt__1nGetGlyphs
  (JNIEnv* env, jclass jclass, jlong ptr, jshortArray resultArray) {
    SkTextBlob* instance = reinterpret_cast<SkTextBlob*>(static_cast<uintptr_t>(ptr));
    SkTextBlob::Iter iter(*instance);
    SkTextBlob::Iter::Run run;

    size_t stored = 0;
    jshort * shorts = env->GetShortArrayElements(resultArray, nullptr);
    while (iter.next(&run)) {
        memcpy(shorts + stored, run.fGlyphIndices, run.fGlyphCount * sizeof(uint16_t));
        stored += run.fGlyphCount;
    }
    env->ReleaseShortArrayElements(resultArray, shorts, 0);
}

extern "C" JNIEXPORT jint JNICALL Java_org_jetbrains_skia_TextBlobKt__1nGetPositionsLength
  (JNIEnv* env, jclass jclass, jlong ptr) {
    SkTextBlob* instance = reinterpret_cast<SkTextBlob*>(static_cast<uintptr_t>(ptr));
    SkTextBlob::Iter iter(*instance);
    SkTextBlob::Iter::Run run;
    jint count = 0;
    while (iter.next(&run)) {
        // run.fGlyphIndices points directly to runRecord.glyphBuffer(), which comes directly after RunRecord itself
        auto runRecord = reinterpret_cast<const RunRecordClone*>(run.fGlyphIndices) - 1;
        unsigned scalarsPerGlyph = RunRecordClone::ScalarsPerGlyph(runRecord->positioning());
        count += run.fGlyphCount * scalarsPerGlyph;
    }
    return count;
}

extern "C" JNIEXPORT void JNICALL Java_org_jetbrains_skia_TextBlobKt__1nGetPositions
  (JNIEnv* env, jclass jclass, jlong ptr, jfloatArray resultArray) {
    SkTextBlob* instance = reinterpret_cast<SkTextBlob*>(static_cast<uintptr_t>(ptr));
    SkTextBlob::Iter iter(*instance);
    SkTextBlob::Iter::Run run;
    size_t stored = 0;

    jfloat* positions = env->GetFloatArrayElements(resultArray, 0);

    while (iter.next(&run)) {
        // run.fGlyphIndices points directly to runRecord.glyphBuffer(), which comes directly after RunRecord itself
        auto runRecord = reinterpret_cast<const RunRecordClone*>(run.fGlyphIndices) - 1;
        unsigned scalarsPerGlyph = RunRecordClone::ScalarsPerGlyph(runRecord->positioning());
        memcpy(&positions[stored], runRecord->posBuffer(), run.fGlyphCount * scalarsPerGlyph * sizeof(SkScalar));
        stored += run.fGlyphCount * scalarsPerGlyph;
    }

    env->ReleaseFloatArrayElements(resultArray, positions, 0);
}

extern "C" JNIEXPORT jintArray JNICALL Java_org_jetbrains_skia_TextBlobKt__1nGetClusters
  (JNIEnv* env, jclass jclass, jlong ptr) {
    SkTextBlob* instance = reinterpret_cast<SkTextBlob*>(static_cast<uintptr_t>(ptr));
    SkTextBlob::Iter iter(*instance);
    SkTextBlob::Iter::Run run;
    std::vector<jint> clusters;
    size_t stored = 0;
    // uint32_t cluster8 = 0;
    uint32_t runStart16 = 0;
    while (iter.next(&run)) {
        // run.fGlyphIndices points directly to runRecord.glyphBuffer(), which comes directly after RunRecord itself
        auto runRecord = reinterpret_cast<const RunRecordClone*>(run.fGlyphIndices) - 1;
        if (!runRecord->isExtended())
            return nullptr;

        skija::UtfIndicesConverter conv(runRecord->textBuffer(), runRecord->textSize());
        clusters.resize(stored + run.fGlyphCount);
        uint32_t* clusterBuffer = runRecord->clusterBuffer();
        for (int i = 0; i < run.fGlyphCount; ++i)
            clusters[stored + i] = runStart16 + conv.from8To16(clusterBuffer[i]);
        runStart16 += conv.from8To16(runRecord->textSize());
        // memcpy(&clusters[stored], runRecord->clusterBuffer(), run.fGlyphCount * sizeof(uint32_t));

        stored += run.fGlyphCount;
    }
    return javaIntArray(env, clusters);
}

extern "C" JNIEXPORT jobject JNICALL Java_org_jetbrains_skia_TextBlobKt__1nGetTightBounds
  (JNIEnv* env, jclass jclass, jlong ptr) {
    SkTextBlob* instance = reinterpret_cast<SkTextBlob*>(static_cast<uintptr_t>(ptr));
    SkTextBlob::Iter iter(*instance);
    SkTextBlob::Iter::Run run;
    auto bounds = SkRect::MakeEmpty();
    SkRect tmpBounds;
    while (iter.next(&run)) {
        // run.fGlyphIndices points directly to runRecord.glyphBuffer(), which comes directly after RunRecord itself
        auto runRecord = reinterpret_cast<const RunRecordClone*>(run.fGlyphIndices) - 1;
        if (runRecord->positioning() != 2) // kFull_Positioning
            return nullptr;

        runRecord->fFont.measureText(runRecord->glyphBuffer(), run.fGlyphCount * sizeof(uint16_t), SkTextEncoding::kGlyphID, &tmpBounds, nullptr);
        SkScalar* posBuffer = runRecord->posBuffer();
        tmpBounds.offset(posBuffer[0], posBuffer[1]);
        bounds.join(tmpBounds);
    }
    return skija::Rect::fromSkRect(env, bounds);
}

extern "C" JNIEXPORT jobject JNICALL Java_org_jetbrains_skia_TextBlobKt__1nGetBlockBounds
  (JNIEnv* env, jclass jclass, jlong ptr) {
    SkTextBlob* instance = reinterpret_cast<SkTextBlob*>(static_cast<uintptr_t>(ptr));
    SkTextBlob::Iter iter(*instance);
    SkTextBlob::Iter::Run run;
    auto bounds = SkRect::MakeEmpty();
    SkFontMetrics metrics;

    while (iter.next(&run)) {
        // run.fGlyphIndices points directly to runRecord.glyphBuffer(), which comes directly after RunRecord itself
        auto runRecord = reinterpret_cast<const RunRecordClone*>(run.fGlyphIndices) - 1;
        if (runRecord->positioning() != 2) // kFull_Positioning
            return nullptr;

        SkScalar* posBuffer = runRecord->posBuffer();
        const SkFont& font = runRecord->fFont;
        font.getMetrics(&metrics);

        SkScalar lastLeft = posBuffer[(run.fGlyphCount - 1) * 2];
        SkScalar lastWidth;
        if (run.fGlyphCount > 1 && SkScalarNearlyEqual(posBuffer[(run.fGlyphCount - 2) * 2], lastLeft))
            lastWidth = 0;
        else
            font.getWidths(&run.fGlyphIndices[run.fGlyphCount - 1], 1, &lastWidth);

        auto runBounds = SkRect::MakeLTRB(posBuffer[0], posBuffer[1] + metrics.fAscent, lastLeft + lastWidth, posBuffer[1] + metrics.fDescent);
        bounds.join(runBounds);
    }
    return skija::Rect::fromSkRect(env, bounds);
}

extern "C" JNIEXPORT jobject JNICALL Java_org_jetbrains_skia_TextBlobKt__1nGetFirstBaseline
  (JNIEnv* env, jclass jclass, jlong ptr) {
    SkTextBlob* instance = reinterpret_cast<SkTextBlob*>(static_cast<uintptr_t>(ptr));
    SkTextBlob::Iter iter(*instance);
    SkTextBlob::Iter::Run run;
    while (iter.next(&run)) {
        // run.fGlyphIndices points directly to runRecord.glyphBuffer(), which comes directly after RunRecord itself
        auto runRecord = reinterpret_cast<const RunRecordClone*>(run.fGlyphIndices) - 1;
        if (runRecord->positioning() != 2) // kFull_Positioning
            return nullptr;

        return javaFloat(env, runRecord->posBuffer()[1]);
    }
    return nullptr;
}

extern "C" JNIEXPORT jobject JNICALL Java_org_jetbrains_skia_TextBlobKt__1nGetLastBaseline
  (JNIEnv* env, jclass jclass, jlong ptr) {
    SkTextBlob* instance = reinterpret_cast<SkTextBlob*>(static_cast<uintptr_t>(ptr));
    SkTextBlob::Iter iter(*instance);
    SkTextBlob::Iter::Run run;
    SkScalar baseline = 0;
    while (iter.next(&run)) {
        // run.fGlyphIndices points directly to runRecord.glyphBuffer(), which comes directly after RunRecord itself
        auto runRecord = reinterpret_cast<const RunRecordClone*>(run.fGlyphIndices) - 1;
        if (runRecord->positioning() != 2) // kFull_Positioning
            return nullptr;

        baseline = std::max(baseline, runRecord->posBuffer()[1]);
    }
    return javaFloat(env, baseline);
}
