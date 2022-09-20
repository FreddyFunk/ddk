#include "fsinfo.hpp"

namespace FSinfoParser {
	enum class OutputMode
	{
		CURRENT,
		ALL,
	};

	enum class ViewMode
	{
		LIST,
		TREE,
	};

	std::string humanReadableSize(std::uintmax_t size);
	std::string getFormattedPrefix(const FSI::FileSystemItem* item);
	std::string getItemInfo(const FSI::FileSystemItem* item, bool fullPath = false);
	std::string FSinfoToStringAsTree(const FSI::FileSystemInfo* const fsinfo);
	std::string FSinfoToStringAsList(const FSI::FileSystemInfo* const fsinfo, OutputMode outputMode, std::size_t limit, bool sorted, bool onlyFiles);
	std::string FSinfoDuplicateList(const FSI::FileSystemInfo* const fsinfo, std::size_t limit, bool sorted);
	std::string FSMetaData(const FSI::FileSystemInfo* const fsinfo);
	std::string summary(const FSI::FileSystemInfo* const fsinfo);
}
