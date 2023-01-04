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

}  // namespace
}  // namespace DDK

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
