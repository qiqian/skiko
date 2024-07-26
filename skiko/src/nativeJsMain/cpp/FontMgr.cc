#include <iostream>
#include "SkData.h"
#include "SkTypeface.h"
#include "SkFontMgr.h"
#include "common.h"
#include "FontMgrWithFallbackWrapper.hh"
#include "FontMgrDefaultFactory.hh"

SKIKO_EXPORT KInt org_jetbrains_skia_FontMgr__1nGetFamiliesCount
  (KNativePointer ptr) {
    SkFontMgr* instance = reinterpret_cast<SkFontMgr*>((ptr));
    return instance->countFamilies();
}


SKIKO_EXPORT KInteropPointer org_jetbrains_skia_FontMgr__1nGetFamilyName
  (KNativePointer ptr, KInt index) {
    SkFontMgr* instance = reinterpret_cast<SkFontMgr*>(ptr);
    SkString familyName;
    instance->getFamilyName(index, &familyName);
    return new SkString(familyName);
}

SKIKO_EXPORT KNativePointer org_jetbrains_skia_FontMgr__1nMakeStyleSet
  (KNativePointer ptr, KInt index) {
    SkFontMgr* instance = reinterpret_cast<SkFontMgr*>(ptr);
    SkFontStyleSet* styleSet = instance->createStyleSet(index).release();

    return reinterpret_cast<KNativePointer>(styleSet);
}


SKIKO_EXPORT KNativePointer org_jetbrains_skia_FontMgr__1nMatchFamily
  (KNativePointer ptr, KInteropPointer familyNameStr) {
    SkFontMgr* instance = reinterpret_cast<SkFontMgr*>(ptr);
    SkFontStyleSet* styleSet = instance->matchFamily(reinterpret_cast<char *>(familyNameStr)).release();
    return reinterpret_cast<KNativePointer>(styleSet);
}

SKIKO_EXPORT KNativePointer org_jetbrains_skia_FontMgr__1nMatchFamilyStyle
  (KNativePointer ptr, KInteropPointer familyNameStr, KInt fontStyle) {
    SkFontMgr* instance = reinterpret_cast<SkFontMgr*>(ptr);
    SkString familyName = skString(familyNameStr);
    SkTypeface* typeface = instance->matchFamilyStyle(familyName.c_str(), skija::FontStyle::fromKotlin(fontStyle)).release();
    return reinterpret_cast<KNativePointer>(typeface);
}

SKIKO_EXPORT KNativePointer org_jetbrains_skia_FontMgr__1nMatchFamilyStyleCharacter
  (KNativePointer ptr, KInteropPointer familyNameStr, KInt fontStyle, KInteropPointerArray bcp47Array, KInt bcp47size, KInt character) {
    SkFontMgr* instance = reinterpret_cast<SkFontMgr*>(ptr);

    SkString familyName = skString(familyNameStr);

    std::vector<SkString> bcp47Strings = skStringVector(bcp47Array, bcp47size);
    std::vector<const char*> bcp47(bcp47Strings.size());
    for (int i = 0; i < bcp47.size(); ++i)
        bcp47[i] = bcp47Strings[i].c_str();

    SkTypeface* typeface = instance->matchFamilyStyleCharacter(familyName.c_str(), skija::FontStyle::fromKotlin(fontStyle), bcp47.data(), (int) bcp47.size(), character).release();

    return reinterpret_cast<KNativePointer>(typeface);
}

SKIKO_EXPORT KNativePointer org_jetbrains_skia_FontMgr__1nMakeFromData
  (KNativePointer ptr, KNativePointer dataPtr, KInt ttcIndex) {
    SkFontMgr* instance = reinterpret_cast<SkFontMgr*>((ptr));
    SkData* data = reinterpret_cast<SkData*>((dataPtr));
    SkTypeface* typeface = instance->makeFromData(sk_ref_sp(data), ttcIndex).release();
    return reinterpret_cast<KNativePointer>(typeface);
}

SKIKO_EXPORT KNativePointer org_jetbrains_skia_FontMgr__1nMakeFromFile
  (KNativePointer ptr, KNativePointer pathStrPtr, KInt ttcIndex) {
    SkFontMgr* instance = reinterpret_cast<SkFontMgr*>(ptr);
    SkString path = skString(pathStrPtr);
    SkTypeface* typeface = instance->makeFromFile(path.c_str(), ttcIndex).release();
    return reinterpret_cast<KNativePointer>(typeface);
}

SKIKO_EXPORT KNativePointer org_jetbrains_skia_FontMgr__1nDefault
  () {
    SkFontMgr* instance = SkFontMgrSkikoDefault().release();
    return reinterpret_cast<KNativePointer>(instance);
}

SKIKO_EXPORT KNativePointer org_jetbrains_skia_FontMgr__1nEmpty
  () {
    SkFontMgr* instance = SkFontMgr::RefEmpty().release();
    return reinterpret_cast<KNativePointer>(instance);
}


SKIKO_EXPORT KNativePointer org_jetbrains_skia_FontMgrWithFallback__1nDefaultWithFallbackFontProvider(KNativePointer fallbackFontProviderPtr) {
    TypefaceFontProviderWithFallback* fallback = reinterpret_cast<TypefaceFontProviderWithFallback*>((fallbackFontProviderPtr));
    FontMgrWithFallbackWrapper* instance = new FontMgrWithFallbackWrapper(sk_ref_sp(fallback));
    return reinterpret_cast<KNativePointer>(instance);
}