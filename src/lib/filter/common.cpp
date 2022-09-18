#include <algorithm>
#include "common.hpp"


namespace FSI::FILTER::COMMON
{
	
	void sortFSitemsBySize(std::vector<FileSystemItem*>& items) {
		std::sort(items.begin(), items.end(), [](const auto lhs, const auto rhs)
		{
			return lhs->getSizeInBytes() > rhs->getSizeInBytes();
		});
	}
	void onlyFiles(std::vector<FileSystemItem*>& items) {
		items.erase(std::remove_if(items.begin(), items.end(),
		[](const FileSystemItem* fsi) { return fsi->getItemType() != FileSystemItemType::REGULAR_FILE; }),
		items.end());
	}
}
