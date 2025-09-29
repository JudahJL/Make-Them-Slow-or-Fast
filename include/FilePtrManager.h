//
// Created by judah on 17-02-2025.
//

#ifndef FILEPTRMANAGER_H
#define FILEPTRMANAGER_H

#pragma warning(push)
#pragma warning(disable: 4'996)

class FilePtrManager
{
public:
    explicit FilePtrManager(const char* path, const char* mode = "rb") noexcept;

    explicit FilePtrManager(const wchar_t* path, const wchar_t* mode = L"rb") noexcept;

    ~FilePtrManager();

    FilePtrManager(const FilePtrManager&)             = delete;
    FilePtrManager& operator= (const FilePtrManager&) = delete;

    FilePtrManager(FilePtrManager&& other) noexcept;

    FilePtrManager& operator= (FilePtrManager&& other) noexcept;

    [[nodiscard]] explicit operator FILE* () noexcept;

    [[nodiscard]] explicit operator FILE* () const noexcept;

    [[nodiscard]] FILE* get() noexcept;

    [[nodiscard]] FILE* get() const noexcept;

    [[nodiscard]] errno_t error() const noexcept;

    [[nodiscard]] explicit operator bool () const noexcept;

private:
    FILE*   fp{};
    errno_t err{};
};

#pragma warning(pop)

#endif  // FILEPTRMANAGER_H
