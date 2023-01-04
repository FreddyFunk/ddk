#include "test_data.hpp"
#include "fsitem.hpp"

#include "gtest/gtest.h"

namespace DDK {
namespace Test {

    TEST(FSItemTest, UseInvalidPath) {
        FileSystemItem broken_item({}, nullptr);
        EXPECT_EQ(broken_item.getError(), FileSystemError::PATH_DOES_NOT_EXIST) 
        << "expected PATH_DOES_NOT_EXIST error";
    }

}  // namespace
}  // namespace DDK

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
