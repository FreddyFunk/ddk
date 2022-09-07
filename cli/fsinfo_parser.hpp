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

	std::string getFormattedPrefix(const FSI::FileSystemItem* item);
	std::string getItemInfo(const FSI::FileSystemItem* item, bool fullPath = false);
	std::string FSinfoToStringAsTree(const FSI::FileSystemInfo* const fsinfo);
	std::string FSinfoToStringAsList(const FSI::FileSystemInfo* const fsinfo, OutputMode outputMode, int limit, bool sorted);
}
