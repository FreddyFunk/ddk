#pragma once

#include "data_types.hpp"
#include "fsitem.hpp"

namespace DDK {
class FileSystemInfo {
  public:
    FileSystemInfo(const std::filesystem::path &path, bool analyzeSymLinks);
    ~FileSystemInfo();

    std::vector<FileSystemItem *> getCurrentDirItems(bool sortedBySize = false,
                                                     bool onlyFiles = false) const;
    std::vector<FileSystemItem *> getAllFileSystemItems(bool sortedBySize = false,
                                                        bool onlyFiles = false) const;
    std::tuple<std::vector<FileSystemItem *>, std::vector<std::size_t>> getDuplicates() const;
    std::vector<std::vector<FileSystemItem *>> getDuplicatesFromCompare(
        const FileSystemInfo *const compare) const;
    std::size_t getDirectoriesCount() const;
    std::size_t getSymlinksCount() const;
    std::size_t getFilesCount() const;
    std::uintmax_t getTotalSize() const;
    std::filesystem::path getRootPath() const;
    bool symlinks() const;

  private:
    FileSystemItem *m_root;
    const bool m_analyzeSymLinks;

    std::vector<FileSystemItem *> getFileSystemItemsRecursive(
        const FileSystemItem *const item) const;
};
} // namespace DDK
