#ifndef __CPPUTILS_FILE_MAPPING_H__
#define __CPPUTILS_FILE_MAPPING_H__

#include <stdint.h>
#include <limits> // UINT64_MAX
#include <string>

#ifdef _MSC_VER
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#define NOGDI
#include <windows.h>
#endif

namespace cpputils {

class FileMapping final {
public:
    static constexpr uint32_t READ = 1;
    static constexpr uint32_t WRITE = 2;

public:
    FileMapping() {}

    ~FileMapping() {
        Destroy();
    }

    FileMapping(FileMapping&&);
    void operator=(FileMapping&&);

    /**
       @param permission MUST be one of: READ, WRITE or READ|WRITE
       @param offset no alignment is required.
    */
    bool Init(const char* filename, uint32_t permission, uint64_t offset = 0,
              uint64_t len = UINT64_MAX, std::string* errmsg = nullptr);
    void Destroy();

    void* data() {
        return m_start;
    }

    const void* data() const {
        return m_start;
    }

    uint64_t size() const {
        return m_size;
    }

    uint32_t permission() const {
        return m_permission;
    }

private:
    void DoMove(FileMapping&&);

private:
    uint32_t m_permission = 0;
    uint64_t m_size = 0;
    void* m_start = nullptr;
    void* m_base = nullptr;

#ifdef _MSC_VER
    HANDLE m_file_handle = nullptr;
    HANDLE m_file_map_handle = nullptr;
#else
    int m_fd = -1;
#endif

private:
    FileMapping(const FileMapping&) = delete;
    FileMapping& operator=(const FileMapping&) = delete;
};

} // namespace cpputils

#endif
