#include "cpputils/file_mapping.h"
#include <cstring>
using namespace std;

#ifndef _MSC_VER
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#endif

namespace cpputils {

void FileMapping::Destroy() {
#ifdef _MSC_VER
    if (m_file_map_handle) {
        UnmapViewOfFile(m_base);
        CloseHandle(m_file_map_handle);
        CloseHandle(m_file_handle);
        m_file_map_handle = nullptr;
    }
#else
    if (m_fd >= 0) {
        munmap(m_base, m_size + ((char*)m_start - (char*)m_base));
        close(m_fd);
        m_fd = -1;
    }
#endif
}

void FileMapping::DoMove(FileMapping&& fm) {
    m_permission = fm.m_permission;
    m_size = fm.m_size;
    m_start = fm.m_start;
    m_base = fm.m_base;

#ifdef _MSC_VER
    m_file_handle = fm.m_file_handle;
    m_file_map_handle = fm.m_file_map_handle;
#else
    m_fd = fm.m_fd;
#endif

    fm.m_permission = 0;
    fm.m_size = 0;
    fm.m_start = nullptr;
    fm.m_base = nullptr;

#ifdef _MSC_VER
    m_file_handle = nullptr;
    m_file_map_handle = nullptr;
#else
    m_fd = -1;
#endif
}

FileMapping::FileMapping(FileMapping&& fm) {
    if (&fm != this) {
        DoMove(std::move(fm));
    }
}

void FileMapping::operator=(FileMapping&& fm) {
    if (&fm != this) {
        Destroy();
        DoMove(std::move(fm));
    }
}

#ifdef _MSC_VER
static constexpr uint32_t MAX_MSG_BUF_SIZE = 1024;

bool FileMapping::Init(const char* filename, uint32_t permission,
                       uint64_t offset, uint64_t len, string* errmsg) {
    if (m_start) {
        if (errmsg) {
            *errmsg = "duplicated init.";
        }
        return false;
    }

    DWORD flags = 0;
    if (permission & FileMapping::READ) {
        flags |= GENERIC_READ;
    }
    if (permission & FileMapping::WRITE) {
        flags |= GENERIC_WRITE;
    }
    m_file_handle = CreateFile(filename, flags, FILE_SHARE_READ, nullptr,
                               OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (m_file_handle == INVALID_HANDLE_VALUE) {
        char message[MAX_MSG_BUF_SIZE];
        FormatMessage(
            FORMAT_MESSAGE_IGNORE_INSERTS | FORMAT_MESSAGE_FROM_SYSTEM, nullptr,
            GetLastError(), 0, message, MAX_MSG_BUF_SIZE, nullptr);
        if (errmsg) {
            *errmsg = message;
        }
        return false;
    }

    DWORD file_size = GetFileSize(m_file_handle, nullptr);
    if (file_size == 0) {
        CloseHandle(m_file_handle);
        return true;
    }

    if (offset >= file_size) {
        if (errmsg) {
            *errmsg = "offset [" + to_string(offset) + "] >= file size [" +
                to_string(file_size) + "]";
        }
        return false;
    }

    auto max_len = file_size - offset;
    if (len > max_len) {
        len = max_len;
    }

    flags = 0;
    if (permission & FileMapping::WRITE) {
        flags = PAGE_READWRITE;
    } else if (permission & FileMapping::READ) {
        flags = PAGE_READONLY;
    }
    /*
      https://docs.microsoft.com/en-us/windows/win32/memory/creating-a-file-mapping-object

      Since it does not cost you any system resources to create a larger file
      mapping object, create a file mapping object that is the size of the file
      (set the dwMaximumSizeHigh and dwMaximumSizeLow parameters of
      CreateFileMapping both to zero) even if you do not expect to view the
      entire file. The cost in system resources comes in creating the views and
      accessing them.
    */
    m_file_map_handle =
        CreateFileMapping(m_file_handle, nullptr, flags, 0, 0, nullptr);
    if (!m_file_map_handle) {
        char message[MAX_MSG_BUF_SIZE];
        FormatMessage(
            FORMAT_MESSAGE_IGNORE_INSERTS | FORMAT_MESSAGE_FROM_SYSTEM, nullptr,
            GetLastError(), 0, message, MAX_MSG_BUF_SIZE, nullptr);
        if (errmsg) {
            *errmsg = message;
        }
        goto errout;
    }

    SYSTEM_INFO sys_info;
    GetSystemInfo(&sys_info);

    const uint64_t mapping_start_offset =
        (offset / sys_info.dwAllocationGranularity) *
        sys_info.dwAllocationGranularity;
    const DWORD file_offset_high = (mapping_start_offset >> 32);
    const DWORD file_offset_low = (mapping_start_offset & 0xffffffff);

    flags = 0;
    if (permission & FileMapping::READ) {
        flags |= FILE_MAP_READ;
    }
    if (permission & FileMapping::WRITE) {
        flags |= FILE_MAP_WRITE;
    }

    m_base =
        MapViewOfFile(m_file_map_handle, flags, file_offset_high,
                      file_offset_low, len + (offset - mapping_start_offset));
    if (!m_base) {
        char message[MAX_MSG_BUF_SIZE];
        FormatMessage(
            FORMAT_MESSAGE_IGNORE_INSERTS | FORMAT_MESSAGE_FROM_SYSTEM, nullptr,
            GetLastError(), 0, message, MAX_MSG_BUF_SIZE, nullptr);
        if (errmsg) {
            *errmsg = message;
        }
        goto errout2;
    }

    m_start = (char*)m_base + (offset - mapping_start_offset);
    m_size = len;
    m_permission = permission;
    return true;

errout2:
    CloseHandle(m_file_map_handle);
    m_file_map_handle = nullptr;
errout:
    CloseHandle(m_file_handle);
    m_file_handle = nullptr;
    return false;
}
#else
bool FileMapping::Init(const char* filename, uint32_t permission,
                       uint64_t offset, uint64_t len, string* errmsg) {
    if (m_start) {
        if (errmsg) {
            *errmsg = "duplicated init";
        }
        return false;
    }

    const uint64_t page_size = sysconf(_SC_PAGE_SIZE);
    const uint64_t mapping_start_offset = (offset / page_size) * page_size;
    uint64_t mapped_len;

    int flags = O_CLOEXEC;
    if ((permission & FileMapping::READ) && (permission & FileMapping::WRITE)) {
        flags |= O_RDWR;
    } else if (permission & FileMapping::WRITE) {
        flags |= O_WRONLY;
    } else if (permission & FileMapping::READ) {
        flags |= O_RDONLY;
    }

    int fd = open(filename, flags);
    if (fd < 0) {
        if (errmsg) {
            *errmsg = strerror(errno);
        }
        return false;
    }

    {
        struct stat file_stat_info;
        memset(&file_stat_info, 0, sizeof(file_stat_info));
        if (fstat(fd, &file_stat_info) < 0) {
            if (errmsg) {
                *errmsg = strerror(errno);
            }
            goto errout;
        }

        uint64_t file_size = file_stat_info.st_size;
        if (file_stat_info.st_size == 0) {
            close(fd);
            fd = -1;
            return true;
        }
        if (offset >= file_size) {
            if (errmsg) {
                *errmsg = "offset [" + to_string(offset) + "] >= file size [" +
                    to_string(file_size) + "].";
            }
            goto errout;
        }

        auto max_len = file_size - offset;
        if (len > max_len) {
            len = max_len;
        }
    }

    flags = 0;
    if (permission & FileMapping::READ) {
        flags |= PROT_READ;
    }
    if (permission & FileMapping::WRITE) {
        flags |= PROT_WRITE;
    }

    mapped_len = len + (offset - mapping_start_offset);
    m_base =
        mmap(NULL, mapped_len, flags, MAP_PRIVATE, fd, mapping_start_offset);
    if (m_base != MAP_FAILED) {
        m_fd = fd;
        m_start = (char*)m_base + (offset - mapping_start_offset);
        m_size = len;
        m_permission = permission;
        return true;
    }

    if (errmsg) {
        *errmsg = strerror(errno);
    }

errout:
    close(fd);
    return false;
}
#endif

}
