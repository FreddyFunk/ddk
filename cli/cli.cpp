#include <iostream>
#include "fsinfo_parser.hpp"
#include "input_params_parser.hpp"

static void printHelpOption() {
	std::cout << "How to use FileSystemInfo:" << std::endl;
}

static bool getPath(const InputParser* const input, std::filesystem::path& selectedPath) {
	if (input->cmdOptionExists("-p")) {
		const std::string& directory = input->getCmdOption("-p");
		if (!directory.empty()) {
			selectedPath = std::filesystem::path(directory);
		}
		else
		{
			return false;
		}
	}
	else
	{
		selectedPath = std::filesystem::current_path();
	}

	return true;
}

static FSinfoParser::DirMode getDirMode(const InputParser* const input) {
	if (!input->cmdOptionExists("-dir"))
	{
		// default
		return FSinfoParser::DirMode::CURRENT;
	}

	const std::string dirMode = input->getCmdOption("-dir");

	if (dirMode == "current")
	{
		return FSinfoParser::DirMode::CURRENT;
	}
	else if (dirMode == "all")
	{
		return FSinfoParser::DirMode::ALL;
	}
	else
	{
		// default
		return FSinfoParser::DirMode::CURRENT;
	}
}

static FSinfoParser::ViewMode getViewMode(const InputParser* const input) {
	if (!input->cmdOptionExists("-view"))
	{
		// default
		return FSinfoParser::ViewMode::LIST;
	}

	const std::string dirMode = input->getCmdOption("-view");

	if (dirMode == "list")
	{
		return FSinfoParser::ViewMode::LIST;
	}
	else if (dirMode == "tree")
	{
		return FSinfoParser::ViewMode::TREE;
	}
	else
	{
		// default
		return FSinfoParser::ViewMode::LIST;
	}
}

static int getLimit(const InputParser* const input) {
	if (!input->cmdOptionExists("-limit"))
	{
		// default
		return -1;
	}

	try
	{
		return std::stoi(input->getCmdOption("-limit"));
	}
	catch (const std::exception&)
	{
		return -1;
	}
}

static void printResults(const FSI::FileSystemInfo* const fsinfo, FSinfoParser::DirMode dirMode, FSinfoParser::ViewMode viewMode, bool sorted, int limit) {
	std::string result{};

	switch (viewMode)
	{
	case FSinfoParser::ViewMode::TREE:
		result = FSinfoParser::FSinfoToStringAsTree(fsinfo);
		break;
	case FSinfoParser::ViewMode::LIST:
	default:
		result = FSinfoParser::FSinfoToStringAsList(fsinfo, dirMode, limit, sorted);
		break;
	}

	std::cout << result << std::endl;
}

int main(int argc, char* argv[]) {
	const InputParser input(argc, argv);

	if (input.cmdOptionExists("-h") || input.cmdOptionExists("--help")) {
		printHelpOption();
		return 0;
	}

	std::filesystem::path path;
	if (!getPath(&input, path))
	{
		std::cout << "ERROR: Custom path was choosen (\"-p\"), but no path was providen." << std::endl;
		return 1;
	}
	if (!std::filesystem::exists(path))
	{
		std::cout << "ERROR: Custom path \"" << path << "\" does not exists." << std::endl;
		return 1;
	}

	const FSinfoParser::DirMode dirMode = getDirMode(&input);
	const FSinfoParser::ViewMode viewMode = getViewMode(&input);
	const bool sorted = input.cmdOptionExists("-sorted");
	const int limit = getLimit(&input);

	const FSI::FileSystemInfo fsinfo(path);

	std::cout << "Results for: " << path << std::endl;
	printResults(&fsinfo, dirMode, viewMode, sorted, limit);

	system("pause");

	return 0;
}