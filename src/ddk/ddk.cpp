#include <algorithm>
#include <fmt/core.h>
#include "fsinfo_parser.hpp"
#include "input_params_parser.hpp"

static void printHelpOption() {
	fmt::print(fmt::format(
R"(DeDuplicationKit (ddk) is a simple command line tool for finding duplicate files at high speed.

Usage: ddk [OPTION]...

Options:
-h            display this help and exit
-p PATH       search for duplicates in PATH (default: current working directory)
--symlinks    follow symbolic links during file search (default: deactivated)

Examples:
ddk
ddk -s
ddk -p "~/Documents" --symlinks
ddk -p "./path/to/files/")"));
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

static void printResultsDedup(const FSI::FileSystemInfo* const fsinfo) {
	std::string result = FSinfoParser::FSinfoDuplicateList(fsinfo);
	
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
	const bool analyzeSymLinks = input.cmdOptionExists("--symlinks");

	// Scan file system structure
	const FSI::FileSystemInfo fsinfo(path, analyzeSymLinks);

    printResultsDedup(&fsinfo);

	return 0;
}
