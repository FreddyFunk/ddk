#include "test_data.hpp"
#include "fsitem.hpp"
#include <algorithm>

#include "gtest/gtest.h"

namespace DDK {
namespace Test {

    // The fixture for testing class FSItem.
    class FSItemTestFileSystemStructures : public testing::TestWithParam<std::tuple<std::size_t, std::size_t, std::size_t>> {
    protected:
        FSItemTestFileSystemStructures(){
            std::filesystem::create_directory(base_path);
            Data::setupDirectory(base_path, directories_per_depth, files_per_directory, recursion_depth);
            if (files_per_directory > 0)
            {
                file_size = std::filesystem::file_size(base_path / ("file_" + std::to_string(0) + ".txt"));
            }   
        }

        ~FSItemTestFileSystemStructures() override {
            std::filesystem::remove_all(base_path);
        }

        void SetUp() override {
            item = new FileSystemItem(base_path, nullptr);
        }

        void TearDown() override {
            delete item;
        }

        FileSystemItem *item = nullptr;
        const std::string test_directory = "ddk_test_data";
        const std::filesystem::path base_path = std::filesystem::current_path() / "" / test_directory;
        const std::size_t recursion_depth = std::get<0>(GetParam());
        const std::size_t directories_per_depth = std::get<1>(GetParam());
        const std::size_t files_per_directory = std::get<2>(GetParam());
        const std::size_t total_directory_count = Data::calcualteTotalDirectoryCount(recursion_depth, directories_per_depth);
        const std::size_t total_file_count = (total_directory_count * files_per_directory) + files_per_directory;
        std::size_t file_size = 0;
    };

    TEST_P(FSItemTestFileSystemStructures, CorrectInitializedRoot) {
        ASSERT_NE(item->getError(), FileSystemError::PATH_DOES_NOT_EXIST);
        ASSERT_NE(item->getError(), FileSystemError::ACCESS_DENIED);
        EXPECT_EQ(item->getError(), FileSystemError::NO_ERROR);
        EXPECT_EQ(item->getParent(), nullptr);
        EXPECT_EQ(item->getRelativeDirDepth(), 0);
        EXPECT_EQ(item->getItemType(), std::filesystem::file_type::directory) << "for path: " << base_path;
        EXPECT_EQ(item->getSizeInBytes(), file_size * total_file_count);
        EXPECT_EQ(item->getItemName(), test_directory);
        EXPECT_EQ(item->getPath(), base_path);
        EXPECT_EQ(item->getPathAsString(), base_path.string());
        EXPECT_EQ(item->getChildFilesCount(), total_file_count);
        EXPECT_EQ(item->getChildSubDirectoriesCount(), total_directory_count);
        EXPECT_EQ(item->getChildSymlinksCount(), 0);
        EXPECT_EQ(item->getChildren().size(), directories_per_depth + files_per_directory);
        EXPECT_EQ(item->getDuplicates().size(), 0);
        EXPECT_EQ(item->getPotentialDuplicates().size(), 0);
        EXPECT_EQ(item->getHash(), 0);
    }

    TEST_P(FSItemTestFileSystemStructures, CorrectInitializedChildSubdirectory) {
        if (directories_per_depth == 0)
        {
            return; // return directly to avoid skipping tests spam in test report.
            GTEST_SKIP() << "Skipping CorrectInitializedChildFileInSubdirectory test since directories_per_depth == 0";
        }

        const auto children = item->getChildren();
        const auto it = std::find_if(children.begin(), children.end(), [](const auto& item) {return item->getItemName() == "dir_0";});
        auto sub = children.at(std::distance(children.begin(), it));

        const std::size_t sub_directory_count = Data::calcualteTotalDirectoryCount(recursion_depth - 1, directories_per_depth);
        const std::size_t total_sub_directory_file_count = (sub_directory_count * files_per_directory) + files_per_directory;        

        ASSERT_NE(sub->getError(), FileSystemError::PATH_DOES_NOT_EXIST);
        ASSERT_NE(sub->getError(), FileSystemError::ACCESS_DENIED);
        EXPECT_EQ(sub->getError(), FileSystemError::NO_ERROR);
        EXPECT_EQ(sub->getParent(), item);
        EXPECT_EQ(sub->getRelativeDirDepth(), 1);
        EXPECT_EQ(sub->getItemType(), std::filesystem::file_type::directory) << "for path: " << sub->getPath();
        EXPECT_EQ(sub->getSizeInBytes(), file_size * total_sub_directory_file_count);
        EXPECT_EQ(sub->getPath(), base_path / sub->getItemName());
        EXPECT_EQ(sub->getPathAsString(), (base_path / sub->getItemName()).string());
        EXPECT_EQ(sub->getChildFilesCount(), total_sub_directory_file_count);
        EXPECT_EQ(sub->getChildSubDirectoriesCount(), sub_directory_count);
        EXPECT_EQ(sub->getChildSymlinksCount(), 0);
        EXPECT_EQ(sub->getChildren().size(), (recursion_depth == 1 ? 0 : directories_per_depth) + files_per_directory);
        EXPECT_EQ(sub->getDuplicates().size(), 0);
        EXPECT_EQ(sub->getPotentialDuplicates().size(), 0);
        EXPECT_EQ(sub->getHash(), 0);
    }

    TEST_P(FSItemTestFileSystemStructures, CorrectInitializedChildFile) {
        if (files_per_directory == 0)
        {
            return; // return directly to avoid skipping tests spam in test report.
            GTEST_SKIP() << "Skipping CorrectInitializedChildFile test since files_per_directory == 0";
        }

        const auto children = item->getChildren();
        const auto file = std::find_if(children.begin(), children.end(), [](const auto& item) {return item->getItemName() == "file_0.txt";});
        auto sub = children.at(std::distance(children.begin(), file));

        ASSERT_NE(sub->getError(), FileSystemError::PATH_DOES_NOT_EXIST);
        ASSERT_NE(sub->getError(), FileSystemError::ACCESS_DENIED);
        EXPECT_EQ(sub->getError(), FileSystemError::NO_ERROR);
        EXPECT_EQ(sub->getParent(), item);
        EXPECT_EQ(sub->getRelativeDirDepth(), 1);
        EXPECT_EQ(sub->getItemType(), std::filesystem::file_type::regular) << "for path: " << sub->getPath();
        EXPECT_EQ(sub->getSizeInBytes(), file_size);
        EXPECT_EQ(sub->getPath(), base_path / sub->getItemName());
        EXPECT_EQ(sub->getPathAsString(), (base_path / sub->getItemName()).string());
        EXPECT_EQ(sub->getChildFilesCount(), 0);
        EXPECT_EQ(sub->getChildSubDirectoriesCount(), 0);
        EXPECT_EQ(sub->getChildSymlinksCount(), 0);
        EXPECT_EQ(sub->getChildren().size(), 0);
        EXPECT_EQ(sub->getDuplicates().size(), 0);
        EXPECT_EQ(sub->getPotentialDuplicates().size(), 0);
        EXPECT_EQ(sub->getHash(), 0);
    }

    TEST_P(FSItemTestFileSystemStructures, CorrectInitializedChildFileInSubdirectory) {
        if (files_per_directory == 0)
        {
            return; // return directly to avoid skipping tests spam in test report.
            GTEST_SKIP() << "Skipping CorrectInitializedChildFileInSubdirectory test since files_per_directory == 0";
        }
        if (directories_per_depth == 0)
        {
            return; // return directly to avoid skipping tests spam in test report.
            GTEST_SKIP() << "Skipping CorrectInitializedChildFileInSubdirectory test since directories_per_depth == 0";
        }

        const auto children = item->getChildren();
        const auto dir = std::find_if(children.begin(), children.end(), [](const auto& item) {return item->getItemName() == "dir_0";});
        auto sub_dir = children.at(std::distance(children.begin(), dir));

        const auto sub_dir_children = sub_dir->getChildren();
        const auto file = std::find_if(sub_dir_children.begin(), sub_dir_children.end(), [](const auto& item) {return item->getItemName() == "file_0.txt";});
        auto sub = sub_dir_children.at(std::distance(sub_dir_children.begin(), file));

        ASSERT_NE(sub->getError(), FileSystemError::PATH_DOES_NOT_EXIST);
        ASSERT_NE(sub->getError(), FileSystemError::ACCESS_DENIED);
        EXPECT_EQ(sub->getError(), FileSystemError::NO_ERROR);
        EXPECT_EQ(sub->getParent(), sub_dir);
        EXPECT_EQ(sub->getRelativeDirDepth(), 2);
        EXPECT_EQ(sub->getItemType(), std::filesystem::file_type::regular) << "for path: " << sub->getPath();
        EXPECT_EQ(sub->getSizeInBytes(), file_size);
        EXPECT_EQ(sub->getPath(), base_path / sub_dir->getItemName() / sub->getItemName());
        EXPECT_EQ(sub->getPathAsString(), (base_path / sub_dir->getItemName() / sub->getItemName()).string());
        EXPECT_EQ(sub->getChildFilesCount(), 0);
        EXPECT_EQ(sub->getChildSubDirectoriesCount(), 0);
        EXPECT_EQ(sub->getChildSymlinksCount(), 0);
        EXPECT_EQ(sub->getChildren().size(), 0);
        EXPECT_EQ(sub->getDuplicates().size(), 0);
        EXPECT_EQ(sub->getPotentialDuplicates().size(), 0);
        EXPECT_EQ(sub->getHash(), 0);
    }

    INSTANTIATE_TEST_SUITE_P(FSItemTestFileSystemStructuresWithParameter,
	FSItemTestFileSystemStructures,
	testing::Combine(testing::Values(1, 2, 3), // recursion_depth
    testing::Values(0, 1, 3), // directories_per_depth
    testing::Values(0, 1, 5) // files_per_directory
    ));

}  // namespace
}  // namespace DDK

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
