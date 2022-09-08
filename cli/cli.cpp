#include <algorithm>
#include <iostream>
#include "fsinfo_parser.hpp"
#include "input_params_parser.hpp"

static void printHelpOption() {
	std::cout << "How to use FileSystemInfo:" << std::endl;
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

static void printResults(const FSI::FileSystemInfo* const fsinfo, FSinfoParser::OutputMode outputMode, FSinfoParser::ViewMode viewMode, bool sorted, int limit, bool onlyFiles) {
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
		std::cout << "ERROR: Custom path " << path << " does not exists." << std::endl;
		return 1;
	}

	const FSinfoParser::OutputMode outputMode = getOutputMode(&input);
	const FSinfoParser::ViewMode viewMode = getViewMode(&input);
	const bool sorted = input.cmdOptionExists("-s");
	const bool analyzeSymLinks = input.cmdOptionExists("--include-symlinks");
	const bool onlyFiles = input.cmdOptionExists("--only-files");
	const int limit = getLimit(&input);

	const FSI::FileSystemInfo fsinfo(path, analyzeSymLinks);

	std::cout << "Results for: " << path << std::endl;
	std::cout << FSinfoParser::FSMetaData(&fsinfo) << std::endl;
	std::cout << FSinfoParser::summary(&fsinfo) << std::endl;
	
	printResults(&fsinfo, outputMode, viewMode, sorted, limit, onlyFiles);

	return 0;
}