#include "fsinfo.hpp"
#include "test_data.hpp"

#include "gtest/gtest.h"

namespace DDK {
namespace Test {

class FSInfoTestFileSystemStructures
    : public testing::TestWithParam<std::tuple<std::size_t, std::size_t, std::size_t>> {
  protected:
    FSInfoTestFileSystemStructures() {
        // base directory
        std::filesystem::create_directory(base_path);
        Data::setupDirectory(base_path, directories_per_depth, files_per_directory,
                             recursion_depth);

        // compare direcrtory
        std::filesystem::create_directory(base_path_compare);

        if (files_per_directory > 0) {
            file_size =
                std::filesystem::file_size(base_path / ("file_" + std::to_string(0) + ".txt"));
        }
    }

    ~FSInfoTestFileSystemStructures() override {
        std::filesystem::remove_all(base_path);
        std::filesystem::remove_all(base_path_compare);
    }

    void SetUp() override { fsinfo = new FileSystemInfo(base_path, false); }

    void TearDown() override { delete fsinfo; }

    const std::string test_directory = "ddk_test_data";
    const std::filesystem::path base_path = std::filesystem::current_path() / "" / test_directory;
    FileSystemInfo *fsinfo;
    const std::size_t recursion_depth = std::get<0>(GetParam());
    const std::size_t directories_per_depth = std::get<1>(GetParam());
    const std::size_t files_per_directory = std::get<2>(GetParam());
    const std::size_t total_directory_count =
        Data::calcualteTotalDirectoryCount(recursion_depth, directories_per_depth);
    const std::size_t total_file_count =
        (total_directory_count * files_per_directory) + files_per_directory;
    std::size_t file_size = 0;

    // compare directory
    const std::string compare_directory = "ddk_test_data_compare";
    const std::filesystem::path base_path_compare =
        std::filesystem::current_path() / "" / compare_directory;
};

TEST_P(FSInfoTestFileSystemStructures, CorrectInitialized) {
    EXPECT_FALSE(fsinfo->symlinks());
    EXPECT_EQ(fsinfo->getRootPath(), base_path);
    EXPECT_EQ(fsinfo->getTotalSize(), total_file_count * file_size);
    EXPECT_EQ(fsinfo->getFilesCount(), total_file_count);
    EXPECT_EQ(fsinfo->getSymlinksCount(), 0);
    EXPECT_EQ(fsinfo->getDirectoriesCount(),
              total_directory_count + 1); // +1: root fsitem is a directory
    EXPECT_EQ(fsinfo->getCurrentDirItems(false, false).size(),
              directories_per_depth + files_per_directory);
    EXPECT_EQ(fsinfo->getCurrentDirItems(true, false).size(),
              directories_per_depth + files_per_directory);
    EXPECT_EQ(fsinfo->getCurrentDirItems(false, true).size(), files_per_directory);
    EXPECT_EQ(fsinfo->getCurrentDirItems(true, true).size(), files_per_directory);
    EXPECT_EQ(fsinfo->getAllFileSystemItems(false, false).size(),
              total_directory_count + total_file_count + 1); // +1: root fsitem is a directory
    EXPECT_EQ(fsinfo->getAllFileSystemItems(true, false).size(),
              total_directory_count + total_file_count + 1); // +1: root fsitem is a directory
    EXPECT_EQ(fsinfo->getAllFileSystemItems(false, true).size(), total_file_count);
    EXPECT_EQ(fsinfo->getAllFileSystemItems(true, true).size(), total_file_count);

    const std::uintmax_t duplicate_count = directories_per_depth > 0 ? files_per_directory : 0;
    EXPECT_EQ(std::get<1>(fsinfo->getDuplicates()).size(), duplicate_count);

    for (const auto range : std::get<1>(fsinfo->getDuplicates())) {
        EXPECT_EQ(range, total_directory_count + 1);
    }
}

TEST_P(FSInfoTestFileSystemStructures, CompareToSingleDuplicateFileInDirectory) {
    Data::setupDirectory(base_path_compare, 0, 1, 1);
    FileSystemInfo fsinfo_compare(base_path_compare, false);
    const auto duplicates = fsinfo->getDuplicatesFromCompare(&fsinfo_compare);

    const std::uintmax_t duplicate_count = files_per_directory > 0 ? 1 : 0;
    EXPECT_EQ(duplicates.size(), duplicate_count);

    if (files_per_directory > 0) {
        for (const auto &dup : duplicates) {
            EXPECT_EQ(dup.size(), (total_directory_count + 1) + 1);
        }
    }
}

TEST_P(FSInfoTestFileSystemStructures, CompareToSingleDuplicateFile) {
    Data::setupDirectory(base_path_compare, 0, 1, 1);
    FileSystemInfo fsinfo_compare(base_path_compare / "file_0.txt", false);
    const auto duplicates = fsinfo->getDuplicatesFromCompare(&fsinfo_compare);

    const std::uintmax_t duplicate_count = files_per_directory > 0 ? 1 : 0;
    EXPECT_EQ(duplicates.size(), duplicate_count);

    if (files_per_directory > 0) {
        for (const auto &dup : duplicates) {
            EXPECT_EQ(dup.size(), (total_directory_count + 1) + 1);
        }
    }
}

TEST_P(FSInfoTestFileSystemStructures, CompareToSingleNonDuplicateFile) {
    std::ofstream outfile(base_path_compare / "file_0.txt");
    outfile << "Different Content..." << std::endl;
    outfile.close();

    FileSystemInfo fsinfo_compare(base_path_compare / "file_0.txt", false);

    const auto duplicates = fsinfo->getDuplicatesFromCompare(&fsinfo_compare);

    EXPECT_EQ(duplicates.size(), 0);
}

TEST_P(FSInfoTestFileSystemStructures, CompareToIdenticalDirectories) {
    Data::setupDirectory(base_path_compare, directories_per_depth, files_per_directory,
                         recursion_depth);

    FileSystemInfo fsinfo_compare(base_path_compare, false);

    const auto duplicates = fsinfo->getDuplicatesFromCompare(&fsinfo_compare);

    EXPECT_EQ(duplicates.size(), files_per_directory);
    EXPECT_EQ(duplicates.size(), total_directory_count > 1
                                     ? std::get<1>(fsinfo->getDuplicates()).size()
                                     : files_per_directory);
    EXPECT_EQ(std::get<1>(fsinfo->getDuplicates()).size(),
              std::get<1>(fsinfo_compare.getDuplicates()).size());

    if (files_per_directory > 0) {
        for (const auto &dup : duplicates) {
            EXPECT_EQ(dup.size(), (total_directory_count + 1) * 2);
        }
    }
}

TEST_P(FSInfoTestFileSystemStructures, CompareToItself) {
    FileSystemInfo fsinfo_compare(base_path, false);

    const auto duplicates = fsinfo->getDuplicatesFromCompare(&fsinfo_compare);

    EXPECT_EQ(duplicates.size(), directories_per_depth > 0 ? files_per_directory : 0);
    EXPECT_EQ(std::get<1>(fsinfo->getDuplicates()).size(),
              std::get<1>(fsinfo_compare.getDuplicates()).size());

    if (files_per_directory > 0) {
        for (const auto &dup : duplicates) {
            EXPECT_EQ(dup.size(), total_directory_count + 1);
        }
    }
}

TEST_P(FSInfoTestFileSystemStructures, CompareToOwnSubdirectory) {
    if (directories_per_depth == 0) {
        return; // return directly to avoid skipping tests spam in test report.
        GTEST_SKIP() << "Skipping CorrectInitializedChildFileInSubdirectory test since "
                        "directories_per_depth == 0";
    }

    FileSystemInfo fsinfo_compare(base_path / "dir_0", false);

    const auto duplicates = fsinfo->getDuplicatesFromCompare(&fsinfo_compare);

    EXPECT_EQ(duplicates.size(), files_per_directory);
    EXPECT_EQ(duplicates.size(), total_directory_count > 1
                                     ? std::get<1>(fsinfo->getDuplicates()).size()
                                     : files_per_directory);
    EXPECT_EQ(std::get<1>(fsinfo_compare.getDuplicates()).size(),
              recursion_depth > 1 ? std::get<1>(fsinfo->getDuplicates()).size() : 0);

    if (files_per_directory > 0) {
        for (const auto &dup : duplicates) {
            EXPECT_EQ(dup.size(), (total_directory_count + 1));
        }
    }
}

INSTANTIATE_TEST_SUITE_P(FSInfoTestFileSystemStructuresWithParameter,
                         FSInfoTestFileSystemStructures,
                         testing::Combine(testing::Values(1, 2, 3), // recursion_depth
                                          testing::Values(0, 1, 3), // directories_per_depth
                                          testing::Values(0, 1, 5)  // files_per_directory
                                          ));

class FSInfoTestFileCompareDirectoryToDirectory
    : public testing::TestWithParam<
          std::
              tuple<std::size_t, std::size_t, std::size_t, std::size_t, std::size_t, std::size_t>> {
  protected:
    FSInfoTestFileCompareDirectoryToDirectory() {
        // base directory
        std::filesystem::create_directory(base_path);
        Data::setupDirectory(base_path, directories_per_depth, files_per_directory,
                             recursion_depth);

        // compare direcrtory
        std::filesystem::create_directory(base_path_compare);
        Data::setupDirectory(base_path_compare, directories_per_depth_compare,
                             files_per_directory_compare, recursion_depth_compare);
        // file size
        if (files_per_directory > 0) {
            file_size =
                std::filesystem::file_size(base_path / ("file_" + std::to_string(0) + ".txt"));
        } else if (files_per_directory_compare > 0) {
            file_size = std::filesystem::file_size(base_path_compare /
                                                   ("file_" + std::to_string(0) + ".txt"));
        }
    }

    ~FSInfoTestFileCompareDirectoryToDirectory() override {
        std::filesystem::remove_all(base_path);
        std::filesystem::remove_all(base_path_compare);
    }

    void SetUp() override {
        fsinfo = new FileSystemInfo(base_path, false);
        fsinfo_compare = new FileSystemInfo(base_path_compare, false);
    }

    void TearDown() override {
        delete fsinfo;
        delete fsinfo_compare;
    }

    // genearl file size
    std::size_t file_size = 0;

    // base directory
    const std::string test_directory = "ddk_test_data";
    const std::filesystem::path base_path = std::filesystem::current_path() / "" / test_directory;
    FileSystemInfo *fsinfo;
    const std::size_t recursion_depth = std::get<0>(GetParam());
    const std::size_t directories_per_depth = std::get<1>(GetParam());
    const std::size_t files_per_directory = std::get<2>(GetParam());
    const std::size_t total_directory_count =
        Data::calcualteTotalDirectoryCount(recursion_depth, directories_per_depth);
    const std::size_t total_file_count =
        (total_directory_count * files_per_directory) + files_per_directory;

    // compare directory
    const std::string compare_directory = "ddk_test_data_compare";
    const std::filesystem::path base_path_compare =
        std::filesystem::current_path() / "" / compare_directory;
    FileSystemInfo *fsinfo_compare;
    const std::size_t recursion_depth_compare = std::get<3>(GetParam());
    const std::size_t directories_per_depth_compare = std::get<4>(GetParam());
    const std::size_t files_per_directory_compare = std::get<5>(GetParam());
    const std::size_t total_directory_count_compare =
        Data::calcualteTotalDirectoryCount(recursion_depth_compare, directories_per_depth_compare);
    const std::size_t total_file_count_compare =
        (total_directory_count_compare * files_per_directory_compare) + files_per_directory_compare;
};

TEST_P(FSInfoTestFileCompareDirectoryToDirectory, DirectoryToDirectory) {
    const auto duplicates = fsinfo->getDuplicatesFromCompare(fsinfo_compare);

    const std::uintmax_t duplicate_count =
        std::min(files_per_directory, files_per_directory_compare);
    EXPECT_EQ(duplicates.size(), duplicate_count);

    for (const auto &dup : duplicates) {
        EXPECT_EQ(dup.size(), duplicate_count > 0 ? (total_directory_count + 1) +
                                                        (total_directory_count_compare + 1)
                                                  : 0);
    }
}

INSTANTIATE_TEST_SUITE_P(
    FSInfoTestFileCompareDirectoryToDirectoryWithParameter,
    FSInfoTestFileCompareDirectoryToDirectory,
    testing::Combine(testing::Values(1, 2, 3), // base directory: recursion_depth
                     testing::Values(0, 1, 3), // base directory: directories_per_depth
                     testing::Values(0, 1, 5), // base directory: files_per_directory
                     testing::Values(1, 2, 3), // compare directory: recursion_depth
                     testing::Values(0, 1, 3), // compare directory: directories_per_depth
                     testing::Values(0, 1, 5)  // compare directory: files_per_directory
                     ));

} // namespace Test
} // namespace DDK

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
