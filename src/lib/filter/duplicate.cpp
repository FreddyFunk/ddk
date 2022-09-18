#include "duplicate.hpp"

namespace FSI
{
    Duplicate::Duplicate(FSI::FileSystemItem* m_first, FSI::FileSystemItem* m_second){
        if (m_first <= m_second)
        {
            this->m_first = m_first;
            this->m_second = m_second;
        }
        else {
            this->m_first = m_second;
            this->m_second = m_first;
        }
    }

    const FSI::FileSystemItem* const Duplicate::getFirst() const {
        return m_first;
    }

    const FSI::FileSystemItem* const Duplicate::getSecond() const {
        return m_second;
    }

    bool Duplicate::operator==(const Duplicate &d) const{
        return this->m_first == d.m_first && this->m_second == d.m_second;
    }

    bool Duplicate::operator<(const Duplicate &d) const{
        return this->m_first < d.m_first && this->m_second < d.m_second;
    }
}
