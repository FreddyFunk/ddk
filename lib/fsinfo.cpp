#include <algorithm>

#include "fsinfo.hpp"

namespace FSI
{
	FileSystemInfo::FileSystemInfo(const std::filesystem::path& path, bool analyzeSymLinks) : m_analyzeSymLinks(analyzeSymLinks),
	m_spaceInfo(std::filesystem::space(path))
	{
		m_anker = new FileSystemItem(path, nullptr, m_analyzeSymLinks);
	}

	FileSystemInfo::~FileSystemInfo() {
		delete m_anker;
	}

	std::vector<FileSystemItem*> FileSystemInfo::getFileSystemItemsRecursive(const FileSystemItem* const item) const {
		std::vector<FileSystemItem*> items(0);

		for (FileSystemItem* const fsitem : item->getChildren())
		{
			items.push_back(fsitem);
			std::vector<FileSystemItem*> children = getFileSystemItemsRecursive(fsitem);
			items.insert(items.end(), children.begin(), children.end());
		}

		return items;
	}

	void FileSystemInfo::sortFSitemsBySize(std::vector<FileSystemItem*>& items) const {
		std::sort(items.begin(), items.end(), [](const auto lhs, const auto rhs)
			{
				return lhs->getSizeInBytes() > rhs->getSizeInBytes();
			});
	}

	std::vector<FileSystemItem*> FileSystemInfo::getCurrentDirItems(bool sortedBySize) const
	{
		std::vector<FileSystemItem*> items = m_anker->getChildren();

		if (sortedBySize)
		{
			sortFSitemsBySize(items);
		}

		return items;
	}

	std::vector<FileSystemItem*> FileSystemInfo::getAllFileSystemItems(bool sortedBySize) const {
		std::vector<FileSystemItem*> items = getFileSystemItemsRecursive(m_anker);

		if (sortedBySize)
		{
			sortFSitemsBySize(items);
		}

		return items;
	}

	std::filesystem::space_info FileSystemInfo::getSpaceInfo() const {
		return m_spaceInfo;
	}
}
