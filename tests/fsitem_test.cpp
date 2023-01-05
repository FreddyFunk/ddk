#include <algorithm>
#include <system_error>

#include "test_data.hpp"
#include "fsitem.hpp"

#include "gtest/gtest.h"


namespace DDK {
namespace Test {

    // The fixture for testing class FSItem.
    class FSItemTest : public testing::Test {
    protected:
        FSItemTest(){
            std::filesystem::create_directory(base_path);
        }

        ~FSItemTest() override {
            std::filesystem::remove_all(base_path);
        }

        void SetUp() override {
            //item = new FileSystemItem(base_path, nullptr);
        }

        void TearDown() override {
            delete item;
        }

        FileSystemItem *item = nullptr;
        const std::string test_directory = "ddk_test_data";
        const std::filesystem::path base_path = std::filesystem::current_path() / "" / test_directory;
    };

    TEST_F(FSItemTest, UseInvalidPath) {
        FileSystemItem broken_item({}, nullptr);
        EXPECT_EQ(broken_item.getError(), FileSystemError::PATH_DOES_NOT_EXIST) 
        << "expected PATH_DOES_NOT_EXIST error";
    }

    TEST_F(FSItemTest, SpecialCharacters) {
        const std::string dir_name = "🧪";
        const std::string file_name = "Όταν λείπει η γάτα, χορεύουν τα ποντίκια.txt";

        std::filesystem::create_directory(base_path / dir_name);

        std::ofstream outfile(base_path / dir_name / file_name);
        outfile << "Testing..." << std::endl;
        outfile.close();

        const std::size_t size_of_file = std::filesystem::file_size(base_path / dir_name / file_name);

        item = new FileSystemItem(base_path / dir_name, nullptr);

        EXPECT_EQ(item->getItemName(), dir_name);
        EXPECT_EQ(item->getSizeInBytes(), size_of_file);
        EXPECT_EQ(item->getChildren().front()->getItemName(), file_name);
        EXPECT_EQ(item->getChildren().front()->getSizeInBytes(), size_of_file);
    }


    // The fixture for testing class FSItem.
    class FSItemSymlinkTest : public testing::Test {
    protected:
        FSItemSymlinkTest(){
            // Setup base directory
            std::filesystem::create_directory(base_path);
            Data::setupDirectory(base_path, directories_per_depth, files_per_directory, recursion_depth);
            file_size = std::filesystem::file_size(base_path / ("file_" + std::to_string(0) + ".txt"));

            // Setup directory that will be linked
            std::filesystem::create_directory(symlink_dir_path);
            Data::setupDirectory(symlink_dir_path, directories_per_depth_symlink, files_per_directory_symlink, recursion_depth_symlink);
            file_size_symlink = std::filesystem::file_size(symlink_dir_path / ("file_" + std::to_string(0) + ".txt"));

            // Setup file that will be linked
            std::filesystem::create_directory(symlink2_dir_path);
            std::ofstream outfile(symlink2_dir_path / file_name_symlink2);
            outfile << "Testing..." << std::endl;
            outfile.close();
            file_size_symlink2 = std::filesystem::file_size(symlink2_dir_path / file_name_symlink2);

            // Create symlinks
            try
            {
                std::filesystem::create_symlink(symlink_dir_path, base_path / symlink_dir);
                std::filesystem::create_symlink(symlink2_dir_path / file_name_symlink2, base_path / file_name_symlink2);
            }
            catch (const std::system_error& e)
            {
                // Windows specific error: "create_symlink: A required privilege is not held by the client."
                if (e.code().value() == 1314)
                {
					system_error = "\"create_symlink: A required privilege is not held by the client.\" This is a common issue when testing symlinks on Windows. Try executing this test as an administrator.";
					can_create_windows_symlinks = false;
				}
                else
                {
                    throw;
                }
            }
        }

        ~FSItemSymlinkTest() override {
            // Delete symlink targets
            std::filesystem::remove_all(symlink_dir_path);
            std::filesystem::remove_all(symlink2_dir_path);
            std::filesystem::remove_all(base_path);
        }

        void SetUp() override {
        }

        void TearDown() override {
            delete item;
        }

        FileSystemItem *item = nullptr;

        // Setup base directory
        const std::string test_directory = "ddk_test_data";
        const std::filesystem::path base_path = std::filesystem::current_path() / "" / test_directory;
        const std::size_t recursion_depth = 2;
        const std::size_t directories_per_depth = 2;
        const std::size_t files_per_directory = 2;
        const std::size_t total_directory_count = Data::calcualteTotalDirectoryCount(recursion_depth, directories_per_depth);
        const std::size_t total_file_count = (total_directory_count * files_per_directory) + files_per_directory;
        std::size_t file_size = 0;

        // Setup directory that will be linked
        const std::string symlink_dir = "ddk_test_data_symlink";
        const std::filesystem::path symlink_dir_path = std::filesystem::current_path() / "" / symlink_dir;
        const std::size_t recursion_depth_symlink = 3;
        const std::size_t directories_per_depth_symlink = 1;
        const std::size_t files_per_directory_symlink = 1;
        const std::size_t total_directory_count_symlink = Data::calcualteTotalDirectoryCount(recursion_depth_symlink, directories_per_depth_symlink);
        const std::size_t total_file_count_symlink = (total_directory_count_symlink * files_per_directory_symlink) + files_per_directory_symlink;
        std::size_t file_size_symlink = 0;

        // Setup file that will be linked
        const std::string symlink2_dir = "ddk_test_data_symlink2";
        const std::filesystem::path symlink2_dir_path = std::filesystem::current_path() / "" / symlink2_dir;
        const std::string file_name_symlink2 = "file_from_symlink.txt";
        std::size_t file_size_symlink2 = 0;

        // Windows
        bool can_create_windows_symlinks = true;
        std::string system_error = "";
    };

    TEST_F(FSItemSymlinkTest, IgnoreSymlinks) {
        if (!can_create_windows_symlinks)
        {
            GTEST_SKIP() << system_error;
        }

        item = new FileSystemItem(base_path, nullptr, false);

        EXPECT_TRUE(std::filesystem::is_symlink(base_path / symlink_dir));
        EXPECT_TRUE(std::filesystem::is_symlink(base_path / file_name_symlink2));

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

    TEST_F(FSItemSymlinkTest, SymlinkFileAndSymlinkDirectory) {
        if (!can_create_windows_symlinks)
        {
            GTEST_SKIP() << system_error;
        }

        item = new FileSystemItem(base_path, nullptr, true);

        EXPECT_TRUE(std::filesystem::is_symlink(base_path / symlink_dir));
        EXPECT_TRUE(std::filesystem::is_symlink(base_path / file_name_symlink2));

        ASSERT_NE(item->getError(), FileSystemError::PATH_DOES_NOT_EXIST);
        ASSERT_NE(item->getError(), FileSystemError::ACCESS_DENIED);
        EXPECT_EQ(item->getError(), FileSystemError::NO_ERROR);
        EXPECT_EQ(item->getParent(), nullptr);
        EXPECT_EQ(item->getRelativeDirDepth(), 0);
        EXPECT_EQ(item->getItemType(), std::filesystem::file_type::directory) << "for path: " << base_path;
        EXPECT_EQ(item->getSizeInBytes(), file_size * total_file_count + file_size_symlink * total_file_count_symlink + file_size_symlink2);
        EXPECT_EQ(item->getItemName(), test_directory);
        EXPECT_EQ(item->getPath(), base_path);
        EXPECT_EQ(item->getPathAsString(), base_path.string());
        EXPECT_EQ(item->getChildFilesCount(), total_file_count + total_file_count_symlink + 1);
        EXPECT_EQ(item->getChildSubDirectoriesCount(), total_directory_count + total_directory_count_symlink + 1);
        EXPECT_EQ(item->getChildSymlinksCount(), 2);
        EXPECT_EQ(item->getChildren().size(), directories_per_depth + files_per_directory + 2);
        EXPECT_EQ(item->getDuplicates().size(), 0);
        EXPECT_EQ(item->getPotentialDuplicates().size(), 0);
        EXPECT_EQ(item->getHash(), 0);
    }

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
