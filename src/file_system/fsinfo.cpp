#include "fsinfo.hpp"
#include "filter/common.hpp"
#include "filter/deduplication.hpp"

namespace DDK {
FileSystemInfo::FileSystemInfo(const std::filesystem::path &path, bool analyzeSymLinks) :
    m_analyzeSymLinks(analyzeSymLinks),
    m_root(new FileSystemItem(path, nullptr, analyzeSymLinks)) {}

FileSystemInfo::~FileSystemInfo() { delete m_root; }

std::vector<FileSystemItem *> FileSystemInfo::getFileSystemItemsRecursive(
    const FileSystemItem *const item) const {
    std::vector<FileSystemItem *> items(0);

    for (FileSystemItem *const fsitem : item->getChildren()) {
        items.push_back(fsitem);
        std::vector<FileSystemItem *> children = getFileSystemItemsRecursive(fsitem);
        items.insert(items.end(), children.begin(), children.end());
    }

    return items;
}

std::vector<FileSystemItem *> FileSystemInfo::getCurrentDirItems(bool sortedBySize,
                                                                 bool onlyFiles) const {
    std::vector<FileSystemItem *> items = m_root->getChildren();

    if (onlyFiles) {
        FILTER::COMMON::onlyFiles(items);
    }

    if (sortedBySize) {
        FILTER::COMMON::sortFSitemsBySize(items);
    }

    return items;
}

std::vector<FileSystemItem *> FileSystemInfo::getAllFileSystemItems(bool sortedBySize,
                                                                    bool onlyFiles) const {
    std::vector<FileSystemItem *> items = getFileSystemItemsRecursive(m_root);

    if (onlyFiles) {
        FILTER::COMMON::onlyFiles(items);
    }

    if (sortedBySize) {
        FILTER::COMMON::sortFSitemsBySize(items);
    }

    return items;
}

std::vector<std::vector<FileSystemItem *>> FileSystemInfo::getDuplicates() const {
    auto items = getAllFileSystemItems();
    FILTER::DEDUPLICATION::tagDuplicateBinaries(items);
    return FILTER::DEDUPLICATION::getDuplicateClustersSorted(items);
}

std::vector<std::vector<FileSystemItem *>> FileSystemInfo::getDuplicatesFromCompare(
    const FileSystemInfo *const compare) const {
    auto items = getAllFileSystemItems();
    auto items_compare = compare->getAllFileSystemItems();
    items.insert(items.end(), items_compare.begin(), items_compare.end());

    // TODO: Remove duplicate items in case a subdirectory is compared
    // FILTER::COMMON::removeFSItemsWithIdenticalPath(items);
    FILTER::DEDUPLICATION::tagDuplicateBinaries(items);
    return FILTER::DEDUPLICATION::getDuplicateClustersSorted(items);
}

std::size_t FileSystemInfo::getDirectoriesCount() const {
    return m_root->getChildSubDirectoriesCount();
}

std::size_t FileSystemInfo::getSymlinksCount() const { return m_root->getChildSymlinksCount(); }

std::size_t FileSystemInfo::getFilesCount() const { return m_root->getChildFilesCount(); }

std::uintmax_t FileSystemInfo::getTotalSize() const { return m_root->getSizeInBytes(); }

std::filesystem::path FileSystemInfo::getRootPath() const { return m_root->getPath(); }

bool FileSystemInfo::symlinks() const { return m_analyzeSymLinks; }
} // namespace DDK
