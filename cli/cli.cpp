#include <iostream>
#include <fsinfo.hpp>

static std::string getFormattedPrefix(const FileSystemItem* item) {
	std::string result{};
	std::size_t depth = item->getRelativeDirDepth();

	result += "|";

	for (size_t i = 1; i < depth; i++)
	{
		result += "---";
	}

	result += "> ";

	return result;
}

static std::string getItemInfo(const FileSystemItem* item, bool fullPath = false) {
	std::string itemInfo{};

	switch (item->getError())
	{
	case FileSystemError::ACCESS_DENIED:
		itemInfo += "ERROR: ACCESS DENIED! ";
		break;
	case FileSystemError::PATH_DOES_NOT_EXIST:
		itemInfo += "ERROR: PATH DOES NOT EXIST! ";
		break;
	case FileSystemError::NO_ERROR:
	default:
		break;
	}

	itemInfo += "Size: " + item->getSizeAsString() + " Name: \"";
	
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


std::string FSitemListToStringAsTree(const std::vector<FileSystemItem*>& items) {
	std::string result{};

	for (const FileSystemItem* item : items)
	{
		result += getFormattedPrefix(item) + getItemInfo(item) + "\n";
	}

	return result;
}

std::string FSitemListToStringAsList(const std::vector<FileSystemItem*>& items, std::size_t limit) {
	std::string result{};

	for (size_t index = 0; index < limit && index < items.size(); index++)
	{
		result += getItemInfo(items.at(index), true) + "\n";
	}

	return result;
}

int main() {

	const std::filesystem::path path = std::filesystem::current_path();
	//const std::filesystem::path path("d:");

	FileSystemInfo fsinfo(path);

	std::cout << "Results for: " << path << std::endl;
	//std::cout << FSitemListToStringAsTree(fsinfo.getAllFileSystemItems()) << std::endl;
	std::cout << FSitemListToStringAsList(fsinfo.getAllFileSystemItemsSortedBySize(), 10) << std::endl;

	system("pause");

	return 0;
}