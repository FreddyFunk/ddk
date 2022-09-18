#include <algorithm>
#include <fmt/core.h>
#include "fsinfo_parser.hpp"
#include "input_params_parser.hpp"

static void printHelpOption() {
	fmt::print("How to use FileSystemInfo:\n");
}

static bool getPath(const InputParser* const input, std::filesystem::path& selectedPath) {
	if (!input->cmdOptionExists("-p")) {
		selectedPath = std::filesystem::current_path();
		return true;
	}
	std::string path = input->getCmdOption("-p");
	if (path.empty()) {
		return false;
	}
	const auto tilde = path.find('~');
	if (tilde != std::string::npos){
		const std::string home = std::getenv("HOME");
		path.replace(tilde, 1, home);
	}
	selectedPath = std::filesystem::path(path);

	return true;
}

static FSinfoParser::OutputMode getOutputMode(const InputParser* const input) {
	if (!input->cmdOptionExists("-o"))
	{
		// default
		return FSinfoParser::OutputMode::CURRENT;
	}

	const std::string outputMode = input->getCmdOption("-o");

	if (outputMode == "current")
	{
		return FSinfoParser::OutputMode::CURRENT;
	}
	else if (outputMode == "all")
	{
		return FSinfoParser::OutputMode::ALL;
	}
	else
	{
		// default
		return FSinfoParser::OutputMode::CURRENT;
	}
}

static FSinfoParser::ViewMode getViewMode(const InputParser* const input) {
	if (!input->cmdOptionExists("-v"))
	{
		// default
		return FSinfoParser::ViewMode::LIST;
	}

	const std::string outputMode = input->getCmdOption("-v");

	if (outputMode == "list")
	{
		return FSinfoParser::ViewMode::LIST;
	}
	else if (outputMode == "tree")
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
	if (!input->cmdOptionExists("-l"))
	{
		// default
		return -1;
	}

	try
	{
		return std::stoi(input->getCmdOption("-l"));
	}
	catch (const std::exception&)
	{
		return -1;
	}
}

static void printResultsScan(const FSI::FileSystemInfo* const fsinfo, FSinfoParser::OutputMode outputMode, FSinfoParser::ViewMode viewMode, bool sorted, int limit, bool onlyFiles) {
	std::string result{};

	switch (viewMode)
	{
	case FSinfoParser::ViewMode::TREE:
		result = FSinfoParser::FSinfoToStringAsTree(fsinfo);
		break;
	case FSinfoParser::ViewMode::LIST:
	default:
		result = FSinfoParser::FSinfoToStringAsList(fsinfo, outputMode, limit, sorted, onlyFiles);
		break;
	}
	
	fmt::print("Results for: {}\n", fsinfo->getRootPath().string());
	fmt::print("{}\n", FSinfoParser::FSMetaData(fsinfo));
	fmt::print("{}\n", FSinfoParser::summary(fsinfo));
	fmt::print("{}\n", result);
}

static void printResultsDedup(const FSI::FileSystemInfo* const fsinfo, bool sorted, int limit) {
	std::string result = FSinfoParser::FSinfoDuplicateList(fsinfo, limit, sorted);
	
	fmt::print("Results for: {}\n", fsinfo->getRootPath().string());
	fmt::print("{}\n", FSinfoParser::summary(fsinfo));
	fmt::print("{}\n", result);
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
		fmt::print("ERROR: Custom path was choosen (\"-p\"), but no path was providen.\n");
		return 1;
	}
	if (!std::filesystem::exists(path))
	{
		fmt::print("ERROR: Custom path {} does not exists.\n", path.string());
		return 1;
	}

	// read general parameters 
	const bool sorted = input.cmdOptionExists("--sorted");
	const bool analyzeSymLinks = input.cmdOptionExists("--include-symlinks");
	const bool onlyFiles = input.cmdOptionExists("--only-files");
	const int limit = getLimit(&input);

	// Scan file system structure
	const FSI::FileSystemInfo fsinfo(path, analyzeSymLinks);

	// Scan Mode
	const bool scanMode = input.cmdOptionExists("-scan");

	// Deduplication Mode
	const bool dedupMode = input.cmdOptionExists("-dedup");

	if ((scanMode && dedupMode) || (!scanMode && !dedupMode))
	{
		fmt::print("ERROR: Please select exactly one mode.\n");
		return 1;
	}

	if (scanMode)
	{
		const FSinfoParser::OutputMode outputMode = getOutputMode(&input);
		const FSinfoParser::ViewMode viewMode = getViewMode(&input);
		printResultsScan(&fsinfo, outputMode, viewMode, sorted, limit, onlyFiles);
	}
	else if (dedupMode)
	{
		printResultsDedup(&fsinfo, sorted, limit);
	}

	return 0;
}