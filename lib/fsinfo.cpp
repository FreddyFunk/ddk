#include <algorithm>

#include "fsinfo.hpp"

FileSystemInfo::FileSystemInfo(const std::filesystem::path & path) {
	m_anker = new FileSystemItem(path, nullptr);
}

FileSystemInfo::~FileSystemInfo() {
	delete m_anker;
}

std::vector<FileSystemItem*> FileSystemInfo::getFileSystemItemsRecursive(const FileSystemItem * const item) const {
	std::vector<FileSystemItem*> items(0);

	for (FileSystemItem* const fsitem : item->getChildren())
	{
		items.push_back(fsitem);
		std::vector<FileSystemItem*> children = getFileSystemItemsRecursive(fsitem);
		items.insert(items.end(), children.begin(), children.end());
	}

	return items;
}

std::vector<FileSystemItem*> FileSystemInfo::getAllFileSystemItems() const {
	return getFileSystemItemsRecursive(m_anker);
}

std::vector<FileSystemItem*> FileSystemInfo::getAllFileSystemItemsSortedBySize() const {
	std::vector<FileSystemItem*> items = getAllFileSystemItems();

	std::sort(items.begin(), items.end(), [](const auto lhs, const auto rhs)
		{
			return lhs->getSizeInBytes() > rhs->getSizeInBytes();
		});

	return items;
}
