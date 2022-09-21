#include "filter/common.hpp"
#include "filter/deduplication.hpp"
#include "fsinfo.hpp"

namespace FSI
{
	FileSystemInfo::FileSystemInfo(const std::filesystem::path& path, bool analyzeSymLinks) : m_analyzeSymLinks(analyzeSymLinks),
	m_spaceInfo(std::filesystem::space(path)), m_root(new FileSystemItem(path, nullptr, analyzeSymLinks))
	{
	}

	FileSystemInfo::~FileSystemInfo() {
		delete m_root;
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

	std::vector<FileSystemItem*> FileSystemInfo::getCurrentDirItems(bool sortedBySize, bool onlyFiles) const
	{
		std::vector<FileSystemItem*> items = m_root->getChildren();

		if (onlyFiles)
		{
			FILTER::COMMON::onlyFiles(items);
		}

		if (sortedBySize)
		{
			FILTER::COMMON::sortFSitemsBySize(items);
		}

		return items;
	}

	std::vector<FileSystemItem*> FileSystemInfo::getAllFileSystemItems(bool sortedBySize, bool onlyFiles) const {
		std::vector<FileSystemItem*> items = getFileSystemItemsRecursive(m_root);

		if (onlyFiles)
		{
			FILTER::COMMON::onlyFiles(items);
		}

		if (sortedBySize)
		{
			FILTER::COMMON::sortFSitemsBySize(items);
		}

		return items;
	}

	std::vector<FSI::DATA::Image*> FileSystemInfo::getImages() const {
		std::vector<FileSystemItem*> items = getFileSystemItemsRecursive(m_root);
		FILTER::COMMON::onlyFiles(items);
		std::vector<FSI::DATA::Image*> images{};
		
		for (const auto& item : items)
		{
			FSI::DATA::Image* const image = new FSI::DATA::Image(item);
			if (image->getType() == FSI::DATA::ImageType::UNSUPPORTED)
			{
				delete image;
			}
			else {
				images.push_back(image);
			}
		}
		

		// create image class with FSI pointer and other important metadata (able to process, type, access metadata, histogram, distance to default histogram)
		// create a vector of images
		// scan items for extensions that can be mapped to a supported image type
		// create image from item and store to vector of images


		// extract to other function: is image processable

		// OpenCV (https://docs.opencv.org/4.x/d4/da8/group__imgcodecs.html)
		// Returns true if the specified image can be decoded by OpenCV.
		// cv::haveImageReader (const String &filename);

		// HEIF
		// extract HEIF and LiveImage image to memory

		// return vector of images
		return images;
	}

	std::vector<std::vector<FileSystemItem*>> FileSystemInfo::getDuplicates() const {
		auto items = getAllFileSystemItems();
		FILTER::DEDUPLICATION::tagDuplicateBinaries(items);
		return FILTER::DEDUPLICATION::getDuplicateClustersSorted(items);
	}
	
	std::vector<std::vector<FSI::DATA::Image*>> FileSystemInfo::getPotentialDuplicateImages() const {
		auto items = getImages();
		return {};
	}

	std::filesystem::space_info FileSystemInfo::getSpaceInfo() const {
		return m_spaceInfo;
	}

	std::size_t FileSystemInfo::getDirectoriesCount() const {
		return m_root->getChildSubDirectoriesCount();
	}

	std::size_t FileSystemInfo::getSymlinksCount() const {
		return m_root->getChildSymlinksCount();
	}

	std::size_t FileSystemInfo::getFilesCount() const {
		return m_root->getChildFilesCount();
	}

	std::uintmax_t FileSystemInfo::getTotalSize() const {
		return m_root->getSizeInBytes();
	}

	std::filesystem::path FileSystemInfo::getRootPath() const {
		return m_root->getPath();
	}

	bool FileSystemInfo::symlinks() const {
		return m_analyzeSymLinks;
	}
}
