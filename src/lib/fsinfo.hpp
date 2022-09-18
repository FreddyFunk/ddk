#include "fsitem.hpp"

namespace FSI
{
	class FileSystemInfo
	{
	public:
		FileSystemInfo(const std::filesystem::path& path, bool analyzeSymLinks);
		~FileSystemInfo();

		std::vector<FileSystemItem*> getCurrentDirItems(bool sortedBySize = false, bool onlyFiles = false) const;
		std::vector<FileSystemItem*> getAllFileSystemItems(bool sortedBySize = false, bool onlyFiles = false) const;
		std::filesystem::space_info getSpaceInfo() const;
		std::size_t getDirectoriesCount() const;
		std::size_t getSymlinksCount() const;
		std::size_t getFilesCount() const;
		std::uintmax_t getTotalSize() const;
		bool symlinks() const;

	private:
		const FileSystemItem* m_root;
		const bool m_analyzeSymLinks;
		const std::filesystem::space_info m_spaceInfo;

		std::vector<FileSystemItem*> getFileSystemItemsRecursive(const FileSystemItem* const item) const;
		void sortFSitemsBySize(std::vector<FileSystemItem*>& items) const;
		void filterOnlyFiles(std::vector<FileSystemItem*>& items) const;
	};
}
