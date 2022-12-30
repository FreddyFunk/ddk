//
//  based on mmaplib.h (https://github.com/yhirose/cpp-mmaplib)
//
//  Copyright (c) 2021 Yuji Hirose. All rights reserved.
//  MIT License
//

#include "mmap_file_read_only.hpp"

#if defined(_WIN32)
#include <windows.h>
#define MAP_FAILED NULL
#else
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#endif
#include <stdexcept>

namespace FSI::IO
{
    MMapFileReadOnly::MMapFileReadOnly(const char* path)
    #if defined(_WIN32)
        : m_fileHandle(NULL),
        m_mappingHandle(NULL)
    #else
        : m_fileDiscriptorHandle(-1)
    #endif
        ,
        m_size(0),
        m_address(MAP_FAILED) {
    #if defined(_WIN32)
        m_fileHandle = ::CreateFileA(path, GENERIC_READ, FILE_SHARE_READ, NULL,
                                OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

        if (m_fileHandle == INVALID_HANDLE_VALUE) {
            std::runtime_error("");
        }

        m_size = ::GetFileSize(m_fileHandle, NULL);

        m_mappingHandle = ::CreateFileMapping(m_fileHandle, NULL, PAGE_READONLY, 0, 0, NULL);

        if (m_mappingHandle == NULL) {
            cleanUp();
            std::runtime_error("");
        }

        m_address = ::MapViewOfFile(m_mappingHandle, FILE_MAP_READ, 0, 0, 0);
        #else
        m_fileDiscriptorHandle = open(path, O_RDONLY);
        if (m_fileDiscriptorHandle == -1) {
            std::runtime_error("");
        }

        struct stat sb;
        if (fstat(m_fileDiscriptorHandle, &sb) == -1) {
            cleanUp();
            std::runtime_error("");
        }
        m_size = sb.st_size;

        m_address = ::mmap(NULL, m_size, PROT_READ, MAP_PRIVATE, m_fileDiscriptorHandle, 0);
    #endif

        if (m_address == MAP_FAILED) {
            cleanUp();
            std::runtime_error("");
        }
    }

    MMapFileReadOnly::~MMapFileReadOnly() {
        cleanUp();
    }

    std::size_t MMapFileReadOnly::getSize() const {
        return m_size;
    }

    const char* MMapFileReadOnly::getData() const {
        return (const char*)m_address;
    }

    void MMapFileReadOnly::cleanUp() {
    #if defined(_WIN32)
        if (m_address) {
            ::UnmapViewOfFile(m_address);
            m_address = MAP_FAILED;
        }

        if (m_mappingHandle) {
            ::CloseHandle(m_mappingHandle);
            m_mappingHandle = NULL;
        }

        if (m_fileHandle != INVALID_HANDLE_VALUE) {
            ::CloseHandle(m_fileHandle);
            m_fileHandle = INVALID_HANDLE_VALUE;
        }
    #else
        if (m_address != MAP_FAILED) {
            munmap(m_address, m_size);
            m_address = MAP_FAILED;
        }

        if (m_fileDiscriptorHandle != -1) {
            close(m_fileDiscriptorHandle);
            m_fileDiscriptorHandle = -1;
        }
    #endif
        m_size = 0;
    }
}
