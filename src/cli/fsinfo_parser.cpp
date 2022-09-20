#include "fsinfo_parser.hpp"
#include <sstream>
#include <cmath>

namespace FSinfoParser {
	std::string humanReadableSize(std::uintmax_t size){
		double mantissa = size;
		int i = 0;
		for (; mantissa >= 1024.0; ++i){
			mantissa /= 1024.0;
		}
		mantissa = std::ceil(mantissa * 10.0) / 10.0;
		std::stringstream stream;
		stream << mantissa << " " << "BKMGTPE"[i] << (i == 0 ? "" : "B");
		return stream.str();
	}

	std::string getFormattedPrefix(const FSI::FileSystemItem* item) {
		std::string result{};
		const std::size_t depth = item->getRelativeDirDepth();

		result += "|";

		for (size_t i = 1; i < depth; i++)
		{
			result += "---";
		}

		result += "> ";

		return result;
	}

	std::string getItemInfo(const FSI::FileSystemItem* item, bool fullPath) {
		std::string itemInfo{};

		switch(item->getError())
		{
		case FSI::FileSystemError::ACCESS_DENIED:
			itemInfo += "ERROR: ACCESS DENIED! ";
			break;
		case FSI::FileSystemError::PATH_DOES_NOT_EXIST:
			itemInfo += "ERROR: PATH DOES NOT EXIST! ";
			break;
		case FSI::FileSystemError::NO_ERROR:
		default:
			break;
		}

		itemInfo += "Size: " + humanReadableSize(item->getSizeInBytes()) + " ";
		
		itemInfo += "Type: ";
		switch(item->getItemType()){
		case FSI::FileSystemItemType::DIRECTORY:
			{
				itemInfo += "Directory ";
				break;
			}
		case FSI::FileSystemItemType::REGULAR_FILE:
			{
				itemInfo += "File ";
				break;
			}
		case FSI::FileSystemItemType::SYMLINK:
			{
				itemInfo += "Symlink (Hard) ";
				break;
			}
		case FSI::FileSystemItemType::OTHER:
		default:
			{
				itemInfo += "Other ";
				break;
			}
		}
		
		itemInfo += "Name: \"";

		if (fullPath)
		{
			itemInfo += item->getPathAsString() + "\"";
		}
		else
		{
			itemInfo += item->getItemName() + "\"";
		}

		return itemInfo;
	}

	std::string getDuplicateInfo(const std::vector<FSI::FileSystemItem*>& duplicates) {
		std::string duplicateInfo{};

		duplicateInfo += "Duplicate Group with " + std::to_string(duplicates.size()) + " duplicates detected:\n";
		for (const auto duplicate : duplicates)
		{
			duplicateInfo += getItemInfo(duplicate, true) + "\n";
		}

		return duplicateInfo;
	}

	std::string FSinfoToStringAsTree(const FSI::FileSystemInfo* const fsinfo) {
		std::string result{};

		for (const FSI::FileSystemItem* item : fsinfo->getAllFileSystemItems())
		{
			result += getFormattedPrefix(item) + getItemInfo(item) + "\n";
		}

		return result;
	}

	std::string FSinfoToStringAsList(const FSI::FileSystemInfo* const fsinfo, OutputMode outputMode, std::size_t limit, bool sorted, bool onlyFiles) {
		std::string result{};
		auto items = outputMode == OutputMode::ALL ? fsinfo->getAllFileSystemItems(sorted, onlyFiles) : fsinfo->getCurrentDirItems(sorted, onlyFiles);

		if (limit != 0 && items.size() > limit)
		{
			items.resize(limit);
		}

		for (const auto item : items)
		{
			result += getItemInfo(item, true) + "\n";
		}

		return result;
	}

	std::string FSinfoDuplicateList(const FSI::FileSystemInfo* const fsinfo, std::size_t limit, bool sorted) {
		std::string result{};
		auto duplicates = fsinfo->getDuplicates();

		if (duplicates.empty())
		{
			return "No duplicates found!\n";
		}
		else {
			result += "Duplicate Groups found: " + std::to_string(duplicates.size()) + "\n\n";
		}

		if (limit != 0 && duplicates.size() > limit)
		{
			duplicates.resize(limit);
		}

		for (const auto duplicate : duplicates)
		{
			result += getDuplicateInfo(duplicate) + "\n";
		}

		return result;
	}

	std::string FSMetaData(const FSI::FileSystemInfo* const fsinfo) {
		std::string result{};

		result += "Capacity: " + humanReadableSize(fsinfo->getSpaceInfo().capacity) + "\n";
		result += "Free space: " + humanReadableSize(fsinfo->getSpaceInfo().free) + "\n";
		result += "Available space for a non-priviliged process: " + humanReadableSize(fsinfo->getSpaceInfo().available) + "\n";
	
		return result;
	}

	std::string summary(const FSI::FileSystemInfo* const fsinfo) {
		std::string result{};

		result += "Analyzed files: " + std::to_string(fsinfo->getFilesCount()) + "\n";
		result += "Analyzed subdirectories: " + std::to_string(fsinfo->getDirectoriesCount()) + "\n";
		
		if (fsinfo->symlinks()) {
			result += "Analyzed symlinks: " + std::to_string(fsinfo->getSymlinksCount()) + "\n";
		}
		
		result += "Total size: " + humanReadableSize(fsinfo->getTotalSize()) + "\n";

		return result;
	}
}
