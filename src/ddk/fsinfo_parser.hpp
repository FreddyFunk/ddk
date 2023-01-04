#include "fsinfo.hpp"

namespace DDK::FSInfoParser {
	std::string humanReadableSize(std::uintmax_t size);
	std::string getItemInfo(const DDK::FileSystemItem* item, bool fullPath = false);
	std::string FSinfoDuplicateList(const DDK::FileSystemInfo* const fsinfo);
	std::string summary(const DDK::FileSystemInfo* const fsinfo);
}
