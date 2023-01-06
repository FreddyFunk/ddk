#include <array>

#include "fsitem.hpp"

namespace DDK {
FileSystemItem::FileSystemItem(const std::filesystem::path &path,
                               const FileSystemItem *const parent,
                               bool analyzeSymlinks) :
    m_path(path),
    m_parent(parent),
    m_analyzeSymlinks(analyzeSymlinks),
    m_relativeDirDepth(parent != nullptr ? parent->getRelativeDirDepth() + 1 : 0) {
    m_children = {};
    m_duplicates = {};
    m_potentialDuplicates = {};

    m_size = 0;
    m_childFilesCount = 0;
    m_childSubDirectoriesCount = 0;
    m_childSymlinksCount = std::filesystem::is_symlink(m_path) ? 1 : 0;
    m_hash = 0;

    m_type = std::filesystem::status(path).type();

    if (!std::filesystem::exists(m_path) || m_type == std::filesystem::file_type::not_found) {
        m_error = FileSystemError::PATH_DOES_NOT_EXIST;
        return;
    }

    if (m_type == std::filesystem::file_type::directory) {
        analyzeChildren();
    } else {
        m_size = std::filesystem::file_size(m_path);
    }

    m_error = FileSystemError::NO_ERROR;
}

FileSystemItem::~FileSystemItem() {
    for (const FileSystemItem *item : m_children) {
        delete item;
    }
}

bool FileSystemItem::analyzeChildren() {
    for (const auto &directoryEntry : std::filesystem::directory_iterator(m_path)) {
        FileSystemItem *item;
        try {
            if ((!m_analyzeSymlinks && std::filesystem::is_symlink(directoryEntry)) &&
                (std::filesystem::status(directoryEntry).type() !=
                     std::filesystem::file_type::directory ||
                 std::filesystem::status(directoryEntry).type() !=
                     std::filesystem::file_type::regular)) {
                continue;
            }

            item = new FileSystemItem(directoryEntry.path(), this, m_analyzeSymlinks);
            m_children.push_back(item);
        } catch (const std::exception &e) {
            delete item;
            // permission denied, file deleted, etc.
            // ignore this file system entry and continue
        }
    }
    for (const FileSystemItem *item : m_children) {
        m_size += item->getSizeInBytes();
        m_childSymlinksCount += item->m_childSymlinksCount;

        if (item->getItemType() == std::filesystem::file_type::directory) {
            m_childSubDirectoriesCount += 1 + item->m_childSubDirectoriesCount;
            m_childFilesCount += item->m_childFilesCount;
        } else if (item->getItemType() == std::filesystem::file_type::regular) {
            m_childFilesCount++;
        }
    }
    return true;
}

const FileSystemItem *const FileSystemItem::getParent() const { return m_parent; }

std::uintmax_t FileSystemItem::getSizeInBytes() const { return m_size; }

std::filesystem::path FileSystemItem::getPath() const { return m_path; }

std::size_t FileSystemItem::getRelativeDirDepth() const { return m_relativeDirDepth; }

std::filesystem::file_type FileSystemItem::getItemType() const { return m_type; }

std::vector<FileSystemItem *> FileSystemItem::getChildren() const { return m_children; }

void FileSystemItem::addDuplicate(FileSystemItem *const duplicate) {
    m_duplicates.insert(duplicate);
}

void FileSystemItem::addPotentialDuplicate(FileSystemItem *const duplicate) {
    m_potentialDuplicates.insert(duplicate);
}

std::set<FileSystemItem *> FileSystemItem::getDuplicates() const { return m_duplicates; }

std::set<FileSystemItem *> FileSystemItem::getPotentialDuplicates() const {
    return m_potentialDuplicates;
}

std::string FileSystemItem::getItemName() const { return m_path.filename().string(); }

std::string FileSystemItem::getPathAsString() const { return m_path.string(); }

FileSystemError FileSystemItem::getError() const { return m_error; }

std::size_t FileSystemItem::getChildFilesCount() const { return m_childFilesCount; }

std::size_t FileSystemItem::getChildSubDirectoriesCount() const {
    return m_childSubDirectoriesCount;
}

std::size_t FileSystemItem::getChildSymlinksCount() const { return m_childSymlinksCount; }

void FileSystemItem::setHash(std::uint64_t hash) { m_hash = hash; }

std::uint64_t FileSystemItem::getHash() const { return m_hash; }
} // namespace DDK
