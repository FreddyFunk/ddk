#include "fsinfo.hpp"
#include "test_data.hpp"

#include "gtest/gtest.h"

namespace DDK {
namespace Test {

// The fixture for testing class FSItem.
class FSInfoTestFileSystemStructures
    : public testing::TestWithParam<std::tuple<std::size_t, std::size_t, std::size_t>> {
  protected:
    FSInfoTestFileSystemStructures() {
        std::filesystem::create_directory(base_path);
        Data::setupDirectory(base_path, directories_per_depth, files_per_directory,
                             recursion_depth);
        if (files_per_directory > 0) {
            file_size =
                std::filesystem::file_size(base_path / ("file_" + std::to_string(0) + ".txt"));
        }
    }

    ~FSInfoTestFileSystemStructures() override { std::filesystem::remove_all(base_path); }

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
};

TEST_P(FSInfoTestFileSystemStructures, CorrectInitialized) {
    EXPECT_FALSE(fsinfo->symlinks());
    EXPECT_EQ(fsinfo->getRootPath(), base_path);
    EXPECT_EQ(fsinfo->getTotalSize(), total_file_count * file_size);
    EXPECT_EQ(fsinfo->getFilesCount(), total_file_count);
    EXPECT_EQ(fsinfo->getSymlinksCount(), 0);
    EXPECT_EQ(fsinfo->getDirectoriesCount(), total_directory_count);
    EXPECT_EQ(fsinfo->getCurrentDirItems(false, false).size(),
              directories_per_depth + files_per_directory);
    EXPECT_EQ(fsinfo->getCurrentDirItems(true, false).size(),
              directories_per_depth + files_per_directory);
    EXPECT_EQ(fsinfo->getCurrentDirItems(false, true).size(), files_per_directory);
    EXPECT_EQ(fsinfo->getCurrentDirItems(true, true).size(), files_per_directory);
    EXPECT_EQ(fsinfo->getAllFileSystemItems(false, false).size(),
              total_directory_count + total_file_count);
    EXPECT_EQ(fsinfo->getAllFileSystemItems(true, false).size(),
              total_directory_count + total_file_count);
    EXPECT_EQ(fsinfo->getAllFileSystemItems(false, true).size(), total_file_count);
    EXPECT_EQ(fsinfo->getAllFileSystemItems(true, true).size(), total_file_count);

    const std::uintmax_t duplicate_count = directories_per_depth > 0 ? files_per_directory : 0;
    EXPECT_EQ(fsinfo->getDuplicates().size(), duplicate_count);

    for (const auto &dup : fsinfo->getDuplicates()) {
        EXPECT_EQ(dup.size(), total_directory_count + 1);
    }
}

INSTANTIATE_TEST_SUITE_P(FSInfoTestFileSystemStructuresWithParameter,
                         FSInfoTestFileSystemStructures,
                         testing::Combine(testing::Values(1, 2, 3), // recursion_depth
                                          testing::Values(0, 1, 3), // directories_per_depth
                                          testing::Values(0, 1, 5)  // files_per_directory
                                          ));

} // namespace Test
} // namespace DDK

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
