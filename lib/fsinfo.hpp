#include "fsitem.hpp"

namespace FSI
{
	class FileSystemInfo
	{
	public:
		FileSystemInfo(const std::filesystem::path& path, bool analyzeSymLinks);
		~FileSystemInfo();

		std::vector<FileSystemItem*> getCurrentDirItems(bool sortedBySize = false) const;
		std::vector<FileSystemItem*> getAllFileSystemItems(bool sortedBySize = false) const;
		std::filesystem::space_info getSpaceInfo() const;

	private:
		const FileSystemItem* m_anker;
		const bool m_analyzeSymLinks;
		const std::filesystem::space_info m_spaceInfo;

		std::vector<FileSystemItem*> getFileSystemItemsRecursive(const FileSystemItem* const item) const;
		void sortFSitemsBySize(std::vector<FileSystemItem*>& items) const;
	};
}
