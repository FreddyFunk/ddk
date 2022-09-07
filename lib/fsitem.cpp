#include <array>

#include "fsitem.hpp"

namespace FSI
{
	FileSystemItem::FileSystemItem(const std::filesystem::path & path, const FileSystemItem * const parent, bool analyzeSymlinks)
		: m_path(path), m_parent(parent), m_analyzeSymlinks(analyzeSymlinks), m_relativeDirDepth(parent != nullptr ? parent->getRelativeDirDepth() + 1 : 0) {
		m_children = std::vector<FileSystemItem*>(0);

		m_size = 0;
		m_type = FileSystemItemType::OTHER;

		if (!std::filesystem::exists(m_path))
		{
			m_error = FileSystemError::PATH_DOES_NOT_EXIST;
			return;
		}

		if (std::filesystem::status(m_path).permissions() == std::filesystem::perms::none)
		{
			auto a = std::filesystem::status(m_path).permissions();
			m_error = FileSystemError::ACCESS_DENIED;
			return;
		}

		if (std::filesystem::is_directory(m_path))
		{
			m_type = FileSystemItemType::DIRECTORY;
			analyzeChildren();
		}
		else if(std::filesystem::is_symlink(m_path)) {
			m_type = FileSystemItemType::SYMLINK;
			if(m_analyzeSymlinks){	
				analyzeChildren();
			}
		}
		else if(std::filesystem::is_regular_file(m_path)) {
			m_type = FileSystemItemType::REGULAR_FILE;
			m_size = std::filesystem::file_size(m_path);
		}
		else {
			m_type = FileSystemItemType::OTHER;
			m_size = std::filesystem::file_size(m_path);
		}

		m_error = FileSystemError::NO_ERROR;
	}

	FileSystemItem::~FileSystemItem() {
		for (const FileSystemItem* item : m_children)
		{
			delete item;
		}
	}

	bool FileSystemItem::analyzeChildren() {
		try
		{
			for (const auto& directoryEntry : std::filesystem::directory_iterator(m_path)) {
				FileSystemItem* item = new FileSystemItem(directoryEntry.path(), this, m_analyzeSymlinks);
				m_children.push_back(item);
			}
		}
		catch (const std::exception& e)
		{
			//std::cout << e.what() << std::endl;
			// TODO : proper error handling
			m_size = 0;
			return false;
		}
		for (const FileSystemItem* item : m_children)
		{
			m_size += item->getSizeInBytes();
		}
		return true;
	}

	const FileSystemItem * const FileSystemItem::getParent() const {
		return m_parent;
	}

	std::uintmax_t FileSystemItem::getSizeInBytes() const {
		return m_size;
	}

	std::string FileSystemItem::getSizeAsString() const {
		double size = static_cast<double>(m_size);

		std::size_t sizeNameIndex = 0;
		const std::array<std::string, 5> sizeNames{ std::string("Bytes"), std::string("KB"), std::string("MB"), std::string("GB"), std::string("TB") };
		while (size / 1024.0 >= 1.0 && sizeNameIndex < sizeNames.size())
		{
			size /= 1024.0;
			sizeNameIndex++;
		}

		return ((std::to_string(size) + " ") + sizeNames[sizeNameIndex]);
	}

	std::filesystem::path FileSystemItem::getPath() const {
		return m_path;
	}

	std::size_t FileSystemItem::getRelativeDirDepth() const
	{
		return m_relativeDirDepth;
	}

	FileSystemItemType FileSystemItem::getItemType() const {
		return m_type;
	}

	std::vector<FileSystemItem*> FileSystemItem::getChildren() const {
		return m_children;
	}

	std::string FileSystemItem::getItemName() const {
		try
		{
			return m_path.filename().string();
		}
		catch (const std::exception& e)
		{
			return std::string(e.what());
		}
	}

	std::string FileSystemItem::getPathAsString() const
	{
		try
		{
			return m_path.string();
		}
		catch (const std::exception& e)
		{
			return std::string(e.what());
		}
	}

	FileSystemError FileSystemItem::getError() const
	{
		return m_error;
	}
}
