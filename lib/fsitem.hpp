#include <filesystem>
#include <vector>
#include <string>

enum class FileSystemError
{
	NO_ERROR,
	ACCESS_DENIED,
	PATH_DOES_NOT_EXIST,
};

class FileSystemItem
{
public:
	FileSystemItem(const std::filesystem::path& path, const FileSystemItem* const parent);
	~FileSystemItem();

	const FileSystemItem* const getParent() const;
	std::filesystem::path getPath() const;
	std::size_t getRelativeDirDepth() const;
	std::uintmax_t getSizeInBytes() const;
	std::string getSizeAsString() const;
	std::string getItemName() const;
	std::string getPathAsString() const;
	FileSystemError getError() const;
	std::vector<FileSystemItem*> getChildren() const;

private:
	const FileSystemItem* const m_parent;
	const std::filesystem::path m_path;
	const std::size_t m_relativeDirDepth;
	std::uintmax_t m_size;
	std::vector<FileSystemItem*> m_children;
	FileSystemError m_error;

	//FileSystemItem(const std::filesystem::path& path, const FileSystemItem* const parent, std::size_t dirDepthLevel);
};