#include <filesystem>
#include <vector>
#include <string>

namespace FSI
{
	enum class FileSystemError
	{
		NO_ERROR,
		ACCESS_DENIED,
		PATH_DOES_NOT_EXIST,
	};

	enum class FileSystemItemType
	{
		REGULAR_FILE,
		DIRECTORY,
		SYMLINK,
		OTHER, // block devices, character devices, IPC pipes/sockets
	};

	class FileSystemItem
	{
	public:
		FileSystemItem(const std::filesystem::path& path, const FileSystemItem* const parent, bool analyzeSymlinks);
		~FileSystemItem();

		const FileSystemItem* const getParent() const;
		std::filesystem::path getPath() const;
		std::size_t getRelativeDirDepth() const;
		FileSystemItemType getItemType() const;
		std::uintmax_t getSizeInBytes() const;
		std::string getSizeAsString() const;
		std::string getItemName() const;
		std::string getPathAsString() const;
		FileSystemError getError() const;
		std::vector<FileSystemItem*> getChildren() const;

	private:
		bool analyzeChildren();

		const FileSystemItem* const m_parent;
		const std::filesystem::path m_path;
		const std::size_t m_relativeDirDepth;
		const bool m_analyzeSymlinks;
		FileSystemItemType m_type;
		std::uintmax_t m_size;
		std::vector<FileSystemItem*> m_children;
		FileSystemError m_error;

		//FileSystemItem(const std::filesystem::path& path, const FileSystemItem* const parent, std::size_t dirDepthLevel);
	};
}
