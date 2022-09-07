#include "fsitem.hpp"

namespace FSI
{
	class FileSystemInfo
	{
	public:
		FileSystemInfo(const std::filesystem::path& path);
		~FileSystemInfo();

		std::vector<FileSystemItem*> getCurrentDirItems(bool sortedBySize = false) const;
		std::vector<FileSystemItem*> getAllFileSystemItems(bool sortedBySize = false) const;

	private:
		const FileSystemItem* m_anker;

		std::vector<FileSystemItem*> getFileSystemItemsRecursive(const FileSystemItem* const item) const;
		void sortFSitemsBySize(std::vector<FileSystemItem*>& items) const;
	};
}
