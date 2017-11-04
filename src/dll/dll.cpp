#define EXPORT __declspec(dllexport)

#include "text/text.hpp"

#define _CRT_SECURE_NO_WARNINGS
#include <Windows.h>
#include "TAPlugin.h"

EXPORT unsigned int __stdcall TAPluginGetVersion(const void *) {
	return TA_PLUGIN_VERSION;
}

static text::Cleaner cleaner(std::vector<text::Replacer>{
        text::Replacer(std::wregex(L"<[^>]+>"), L""),
        text::Replacer(std::wregex(L".*(.+)\\1+"), L"$1")
        });

std::wstring buffer;

// Can return null if does nothing to string.
EXPORT wchar_t * __stdcall TAPluginModifyStringPreSubstitution(wchar_t *in) {
    auto result = cleaner.clean(std::wstring(in));

    if (result.has_value()) {
        buffer.swap(*result);
        return const_cast<wchar_t*>(buffer.c_str());
    }
    else {
        return nullptr;
    }
}

// Frees strings returned by previous function.
EXPORT void __stdcall TAPluginFree(void) {
    buffer.clear();
}
