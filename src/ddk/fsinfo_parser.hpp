#include "fsinfo.hpp"

namespace FSinfoParser {
	std::string humanReadableSize(std::uintmax_t size);
	std::string getItemInfo(const FSI::FileSystemItem* item, bool fullPath = false);
	std::string FSinfoDuplicateList(const FSI::FileSystemInfo* const fsinfo);
	std::string summary(const FSI::FileSystemInfo* const fsinfo);
}
