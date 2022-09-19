#pragma once

#include <filesystem>
#include <vector>
#include <string>
#include <set>

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
		std::string getItemName() const;
		std::string getPathAsString() const;
		FileSystemError getError() const;
		std::size_t getChildFilesCount() const;
		std::size_t getChildSubDirectoriesCount() const;
		std::size_t getChildSymlinksCount() const;
		std::vector<FileSystemItem*> getChildren() const;
		void addDuplicate(FileSystemItem* const duplicate);
		void addPotentialDuplicate(FileSystemItem* const duplicate);
		std::set<FileSystemItem*> getDuplicates() const;
		std::set<FileSystemItem*> getPotentialDuplicates() const;
		void setHash(std::uint64_t hash);
		std::uint64_t getHash() const;

	private:
		bool analyzeChildren();

		const FileSystemItem* const m_parent;
		const std::filesystem::path m_path;
		const std::size_t m_relativeDirDepth;
		const bool m_analyzeSymlinks;
		// TODO: Make m_type constant
		FileSystemItemType m_type;
		std::uintmax_t m_size;
		std::uint64_t m_hash;
		std::size_t m_childFilesCount;
		std::size_t m_childSubDirectoriesCount;
		std::size_t m_childSymlinksCount;
		std::vector<FileSystemItem*> m_children;
		std::set<FileSystemItem*> m_duplicates;
		std::set<FileSystemItem*> m_potentialDuplicates;
		FileSystemError m_error;
	};
}
