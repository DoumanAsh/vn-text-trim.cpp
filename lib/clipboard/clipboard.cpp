#include "clipboard.hpp"

#if (_MSC_VER)
#pragma warning(push)
#pragma warning(disable: 4577)
#endif
#include <stdexcept>
#if (_MSC_VER)
#pragma warning(pop)
#endif

#include <windows.h>

Clipboard::Clipboard() {
    if (OpenClipboard(0) == 0) throw std::runtime_error("Failed to open clipboard");
}

Clipboard::~Clipboard() {
    //The most likely case when CloseClipboard can fail is when it hasn't been opened.
    //So just ignore.
    (void)CloseClipboard();
}

bool Clipboard::empty() const {
    return (bool)EmptyClipboard();
}

size_t Clipboard::size(unsigned int format) const {
    const HANDLE clipboard_data = GetClipboardData(format);

    return clipboard_data ? (size_t)GlobalSize(clipboard_data) : 0;
}

size_t Clipboard::get(unsigned int format, char* buf, size_t len) const {
    const HANDLE clipboard_data = GetClipboardData(format);

    if (clipboard_data == NULL) {
        return 0;
    }
    else {
        const char *clipboard_mem = static_cast<const char*>(GlobalLock(clipboard_data));

        if (clipboard_mem == NULL) return 0;

        const size_t clipboard_size = (size_t)GlobalSize(clipboard_data);
        const size_t copy_size = clipboard_size > len ? len : clipboard_size;

        (void)memcpy(buf, clipboard_mem, copy_size);

        (void)GlobalUnlock(clipboard_data);
        return copy_size;
    }
}

std::string Clipboard::get_string() const {
    std::string result;
    const HANDLE clipboard_data = GetClipboardData(CF_TEXT);

    if (clipboard_data == NULL) {
        return result;
    }
    else {
        const char *clipboard_mem = static_cast<const char*>(GlobalLock(clipboard_data));

        if (clipboard_mem == NULL) return result;

        result.append(clipboard_mem);
        (void)GlobalUnlock(clipboard_data);
        return result;
    }
}

std::wstring Clipboard::get_wstring() const {
    std::wstring result;
    const HANDLE clipboard_data = GetClipboardData(CF_UNICODETEXT);

    if (clipboard_data == NULL) {
        return result;
    }
    else {
        const wchar_t *clipboard_mem = reinterpret_cast<const wchar_t*>(GlobalLock(clipboard_data));

        if (clipboard_mem == NULL) return result;

        result.append(clipboard_mem);
        (void)GlobalUnlock(clipboard_data);
        return result;
    }
}

bool Clipboard::set(unsigned int format, const char* buf, size_t len) const {
    const UINT alloc_flags = GHND;
    const HGLOBAL alloc_handle = GlobalAlloc(alloc_flags, len);

    if (alloc_handle == NULL) return false;

    char *alloc_mem = (char*)GlobalLock(alloc_handle);

    (void)memcpy(alloc_mem, buf, len);
    (void)GlobalUnlock(alloc_handle);
    (void)this->empty();

    //On success SetClipboardData owns our memory.
    //But otherwise we must clean it.
    if (SetClipboardData(format, alloc_handle) == NULL) {
        (void)GlobalFree(alloc_handle);
        return false;
    }

    return true;
}

bool Clipboard::set_string(const char* text) const {
    const size_t text_len = strlen(text) + 1;
    return this->set(CF_TEXT, text, text_len);
}

bool Clipboard::set_string(const std::string& text) const {
    const size_t text_len = text.size() + 1;
    return this->set(CF_TEXT, text.c_str(), text_len);
}

bool Clipboard::set_string(const wchar_t* text) const {
    const size_t text_len = (wcslen(text) + 1) * sizeof(wchar_t);
    return this->set(CF_UNICODETEXT, reinterpret_cast<const char*>(text), text_len);
}

bool Clipboard::set_string(const std::wstring& text) const {
    const size_t text_len = (text.size() + 1) * sizeof(wchar_t);
    return this->set(CF_UNICODETEXT, reinterpret_cast<const char*>(text.c_str()), text_len);
}

bool Clipboard::is_format_avail(unsigned int format) {
    return IsClipboardFormatAvailable(format) != 0;
}

unsigned long Clipboard::seq_num() {
    return GetClipboardSequenceNumber();
}

unsigned long Clipboard::count_formats() {
    return CountClipboardFormats();
}

unsigned long Clipboard::register_format(const wchar_t* name) {
    return RegisterClipboardFormatW(name);
}

unsigned long Clipboard::register_format(const std::wstring& name) {
    return RegisterClipboardFormatW(name.c_str());
}

/**
 * @return Name of predefined format.
 */
static inline int format_predefined(UINT format, wchar_t* buffer, size_t size) {
    int result = 0;
#define CAT(_left, _right)   _left##_right
#define FORMATTER(_name) case _name: result = swprintf(buffer, size, CAT(L, #_name)); break
    switch (format) {
        FORMATTER(CF_BITMAP);
        FORMATTER(CF_DIB);
        FORMATTER(CF_DIBV5);
        FORMATTER(CF_DIF);
        FORMATTER(CF_DSPBITMAP);
        FORMATTER(CF_DSPENHMETAFILE);
        FORMATTER(CF_DSPMETAFILEPICT);
        FORMATTER(CF_DSPTEXT);
        FORMATTER(CF_ENHMETAFILE);
        FORMATTER(CF_HDROP);
        FORMATTER(CF_LOCALE);
        FORMATTER(CF_METAFILEPICT);
        FORMATTER(CF_OEMTEXT);
        FORMATTER(CF_OWNERDISPLAY);
        FORMATTER(CF_PALETTE);
        FORMATTER(CF_PENDATA);
        FORMATTER(CF_RIFF);
        FORMATTER(CF_SYLK);
        FORMATTER(CF_TEXT);
        FORMATTER(CF_WAVE);
        FORMATTER(CF_TIFF);
        FORMATTER(CF_UNICODETEXT);
        default:
            if (format >= CF_GDIOBJFIRST && format <= CF_GDIOBJLAST) {
                result = swprintf(buffer, size, L"CF_GDIOBJ%d", format - CF_GDIOBJFIRST);
            } else if (format >= CF_PRIVATEFIRST && format <= CF_PRIVATELAST) {
                result = swprintf(buffer, size, L"CF_PRIVATE%d", format - CF_PRIVATEFIRST);
            }
            else {
                return 0;
            }
    }
#undef FORMATTER
#undef CAT
    /**
     * In truncatation case null isn't appended on Win.
     * So append by ourself and do not include null char to result.
     */
    if (result == -1) {
        result = (int)size - 1;
        buffer[result] = 0;
    }
    return result;
}

/**
 * @return Name of user's registered format
 */
static inline int format_custom(UINT format, wchar_t* buffer, size_t size) {
    return GetClipboardFormatNameW(format, buffer, (int)size);
}

int Clipboard::get_format_name(unsigned long format, wchar_t* buffer, size_t len) {
    return format > 0xC000 ? format_custom(format, buffer, len) : format_predefined(format, buffer, len);
}

std::wstring Clipboard::get_format_name(unsigned long format) {
    wchar_t buffer[120] = {0};
    Clipboard::get_format_name(format, buffer, sizeof(buffer) / sizeof(buffer[0]));
    return std::wstring(buffer);
}

#include <iostream>

ClipboardMaster::ClipboardMaster(cb_t cb) noexcept : cb(cb) {}

void ClipboardMaster::run() {
    const auto window = CreateWindowExW(0, L"STATIC", NULL , 0,
                                        CW_USEDEFAULT, CW_USEDEFAULT,
                                        CW_USEDEFAULT, CW_USEDEFAULT,
                                        HWND_MESSAGE, NULL, NULL, NULL);

    if (window == nullptr) {
        std::cerr << "Failed to create clipboard window. Error: " << GetLastError() << "\n";
        exit(1);
    }

    if (!AddClipboardFormatListener(window)) {
        std::cerr << "Cannot listen for clipboard updates. Error: " << GetLastError() << "\n";
        exit(1);
    }

    MSG msg;
    BOOL result;
    while ((result = GetMessage(&msg, window, 797, 797)) != 0) {
        if (result == -1)
        {
            std::cerr << "Unexpected error when handling clipboard message. Error: " << GetLastError() << "\n";
        }
        else
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
            this->cb();
        }
    }

    (void)RemoveClipboardFormatListener(window);
    (void)DestroyWindow(window);
}
