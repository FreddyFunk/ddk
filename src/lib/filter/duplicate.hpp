#pragma once

#include "fsitem.hpp"

namespace FSI
{
    class Duplicate {
    public:
        Duplicate(FSI::FileSystemItem* first, FSI::FileSystemItem* second);

        const FSI::FileSystemItem* const getFirst() const;
        const FSI::FileSystemItem* const getSecond() const;

        bool operator==(const Duplicate &d) const;
        bool operator<(const Duplicate &d) const;

    private:
        FSI::FileSystemItem* m_first;
        FSI::FileSystemItem* m_second;
    };
}
