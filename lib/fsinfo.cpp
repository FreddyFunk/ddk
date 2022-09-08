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

	void FileSystemInfo::filterOnlyFiles(std::vector<FileSystemItem*>& items) const {	
		items.erase(std::remove_if(items.begin(), items.end(),
		[](const FileSystemItem* fsi) { return fsi->getItemType() != FileSystemItemType::REGULAR_FILE; }),
		items.end());
	}

	std::vector<FileSystemItem*> FileSystemInfo::getCurrentDirItems(bool sortedBySize, bool onlyFiles) const
	{
		std::vector<FileSystemItem*> items = m_anker->getChildren();

		if (onlyFiles)
		{
			filterOnlyFiles(items);
		}

		if (sortedBySize)
		{
			sortFSitemsBySize(items);
		}

		return items;
	}

	std::vector<FileSystemItem*> FileSystemInfo::getAllFileSystemItems(bool sortedBySize, bool onlyFiles) const {
		std::vector<FileSystemItem*> items = getFileSystemItemsRecursive(m_anker);

		if (onlyFiles)
		{
			filterOnlyFiles(items);
		}

		if (sortedBySize)
		{
			sortFSitemsBySize(items);
		}

		return items;
	}

	std::filesystem::space_info FileSystemInfo::getSpaceInfo() const {
		return m_spaceInfo;
	}

	std::size_t FileSystemInfo::getDirectoriesCount() const {
		return m_anker->getChildSubDirectoriesCount();
	}

	std::size_t FileSystemInfo::getSymlinksCount() const {
		return m_anker->getChildSymlinksCount();
	}

	std::size_t FileSystemInfo::getFilesCount() const {
		return m_anker->getChildFilesCount();
	}

	std::uintmax_t FileSystemInfo::getTotalSize() const {
		return m_anker->getSizeInBytes();
	}

	bool FileSystemInfo::symlinks() const {
		return m_analyzeSymLinks;
	}
}
