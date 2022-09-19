
#pragma once

#include <cstddef>

namespace FSI::IO
{
    class MMapFileReadOnly {
    public:
        MMapFileReadOnly(const char* path);
        ~MMapFileReadOnly();

        std::size_t getSize() const;
        const char* getData() const;

    private:
        void cleanUp();

        std::size_t m_size;
        void* m_address;
        #if defined(_WIN32)
        HANDLE m_mappingHandle;
        HANDLE m_fileHandle;
        #else
        int m_fileDiscriptorHandle;
        #endif
    };
}
