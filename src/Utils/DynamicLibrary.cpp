#include "Utils/DynamicLibrary.hpp"

#if defined(_WIN32)
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#else
#include <dlfcn.h>
#endif

namespace utils
{

DynamicLibrary::DynamicLibrary() = default;

DynamicLibrary::DynamicLibrary(const char* filename) { Open(filename); }

DynamicLibrary::~DynamicLibrary() { Close(); }

DynamicLibrary::DynamicLibrary(DynamicLibrary&& o) noexcept
    : handle(std::exchange(o.handle, nullptr)) {}

DynamicLibrary& DynamicLibrary::operator=(DynamicLibrary&& o) noexcept {
    Close();
    handle = std::exchange(o.handle, nullptr);
    return *this;
}

bool DynamicLibrary::Open(const char* filename) {
#if defined(_WIN32)
    handle = reinterpret_cast<void*>(::LoadLibraryA(filename));
#else
    handle = dlopen(filename, RTLD_NOW);
#endif
    return IsOpen();
}

bool DynamicLibrary::IsOpen() const { return handle != nullptr; }

void DynamicLibrary::Close() {
    if (IsOpen()) {
#if defined(_WIN32)
        ::FreeLibrary(reinterpret_cast<HMODULE>(handle));
#else
        dlclose(handle);
#endif
        handle = nullptr;
    }
}

void* DynamicLibrary::GetSymbolAddr(const char* name) const {
#if defined(_WIN32)
    return reinterpret_cast<void*>(
        ::GetProcAddress(reinterpret_cast<HMODULE>(handle), name));
#else
    return reinterpret_cast<void*>(dlsym(handle, name));
#endif
}

}; // namespace wallpaper
