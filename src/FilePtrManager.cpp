//
// Created by Judah on 01-10-2025.
//
#include "FilePtrManager.h"

#pragma warning(push)
#pragma warning(disable: 4'996)

FilePtrManager::FilePtrManager(const char* path, const char* mode) noexcept:
    err(fopen_s(&fp, path, mode)) {
    // ReSharper disable CppDeprecatedEntity
    if(err != 0) {
        SPDLOG_ERROR("Warning: Failed to open file '{}' pointer. Error: {}", path, strerror(err));
    }
}

FilePtrManager::FilePtrManager(const wchar_t* path, const wchar_t* mode) noexcept:
    err(_wfopen_s(&fp, path, mode)) {
    if(err != 0) {
        wchar_t buffer[2'048];

        swprintf_s(buffer, std::size(buffer), L"Failed to open file '%ls' pointer. Error: %ls", path, _wcserror(err));
        SPDLOG_ERROR(buffer);
    }
}

FilePtrManager::~FilePtrManager() {
    if(fp) {
        if(fclose(fp) == EOF) { SPDLOG_ERROR("Failed to close file pointer: {}", strerror(errno)); }
    }
}

FilePtrManager::FilePtrManager(FilePtrManager&& other) noexcept:
    fp(other.fp), err(other.err) {
    other.fp  = nullptr;
    other.err = 0;
}

FilePtrManager& FilePtrManager::operator= (FilePtrManager&& other) noexcept {
    if(this != &other) {
        if(fp) {
            if(fclose(fp) == EOF) {
                SPDLOG_ERROR("Failed to close file pointer in move constructor: {}", strerror(errno));
            }
        }
        fp        = other.fp;
        err       = other.err;
        other.fp  = nullptr;
        other.err = 0;
    }
    return *this;
}

#pragma warning(pop)

FilePtrManager::operator FILE* () noexcept { return fp; }

FilePtrManager::operator FILE* () const noexcept { return fp; }

FILE* FilePtrManager::get() noexcept { return fp; }

FILE* FilePtrManager::get() const noexcept { return fp; }

errno_t FilePtrManager::error() const noexcept { return err; }

FilePtrManager::operator bool () const noexcept { return fp != nullptr && err == 0; }
