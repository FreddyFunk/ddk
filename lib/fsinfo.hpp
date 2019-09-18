#include "fsitem.hpp"

class FileSystemInfo
{
public:
	FileSystemInfo(const std::filesystem::path& path);
	~FileSystemInfo();

	std::vector<FileSystemItem*> getAllFileSystemItems() const;
	std::vector<FileSystemItem*> getAllFileSystemItemsSortedBySize() const;

private:
	const FileSystemItem* m_anker;

	std::vector<FileSystemItem*> getFileSystemItemsRecursive(const FileSystemItem* const item) const;
};