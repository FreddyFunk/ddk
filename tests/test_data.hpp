#include <bitset>
#include <cassert>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

namespace DDK {
namespace Test {
namespace Data {

    // recursion_depth = 1;
    // directories_per_depth = 3;
    // files_per_directory = 5;
    // Base directory:
    // ├── file_0.txt
    // ├── file_1.txt
    // ├── file_2.txt
    // ├── file_3.txt
    // └── file_4.txt
    // ├── dir_0
    // │   ├── file_0.txt
    // │   ├── file_1.txt
    // │   ├── file_2.txt
    // │   ├── file_3.txt
    // │   └── file_4.txt
    // ├── dir_1
    // │   ├── file_0.txt
    // │   ├── file_1.txt
    // │   ├── file_2.txt
    // │   ├── file_3.txt
    // │   └── file_4.txt
    // ├── dir_2
    //     ├── file_0.txt
    //     ├── file_1.txt
    //     ├── file_2.txt
    //     ├── file_3.txt
    //     └── file_4.txt

    // recursion_depth = 3;
    // directories_per_depth = 3;
    // files_per_directory = 2;
    // Base directory:
    // ├── file_0.txt
    // ├── file_1.txt
    // ├── dir_0
    // │   ├── file_0.txt
    // │   ├── file_1.txt
    // │   ├── dir_0
    // │   │   ├── file_0.txt
    // │   │   └── file_1.txt
    // │   └── dir_1
    // │       ├── file_0.txt
    // │       └── file_1.txt
    // ├── dir_1
    // │   ├── file_0.txt
    // │   ├── file_1.txt
    // │   ├── dir_0
    // │   │   ├── file_0.txt
    // │   │   └── file_1.txt
    // │   └── dir_1
    // │       ├── file_0.txt
    // │       └── file_1.txt
    // ├── dir_2
    //     ├── file_0.txt
    //     ├── file_1.txt
    //     ├── dir_0
    //     │   ├── file_0.txt
    //     │   └── file_1.txt
    //     └── dir_1
    //         ├── file_0.txt
    //         └── file_1.txt


    void createFiles(std::filesystem::path current_path, std::size_t files_per_directory){
        for (std::size_t f = 0; f < files_per_directory; f++)
        {
            const std::string file_name = "file_" + std::to_string(f) + ".txt";
            std::ofstream outfile(current_path / file_name);
            outfile << std::bitset<64>(f).to_string() << std::endl;
            outfile.close();
        }
    }

    // recursion_depth <= 0 is not allowed
    void setupDirectory(std::filesystem::path current_path, std::size_t directories_per_depth, std::size_t files_per_directory, 
        std::size_t recursion_depth, std::size_t current_recursion_depth = 0){
        assert(recursion_depth != 0);

        if (directories_per_depth == 0)
        {
            createFiles(current_path, files_per_directory);
            return;
        }
        
        for (std::size_t dir = 0; dir < directories_per_depth; dir++)
        {
            createFiles(current_path, files_per_directory);

            const std::string dir_name = "dir_" + std::to_string(dir);
            if (current_recursion_depth < recursion_depth)
            {
                std::filesystem::create_directory(current_path / dir_name);
                setupDirectory(current_path / dir_name, directories_per_depth, files_per_directory, recursion_depth, current_recursion_depth + 1);
            }
        }
    }

    constexpr std::size_t calcualteTotalDirectoryCount(std::size_t recursion_depth, std::size_t directories_per_depth){
        // For each level of recursion, the number of directories created is equal to directories_per_depth. 
        // Therefore, the total number of directories created at a given recursion depth d is directories_per_depth^d.
        // The total number of directories created for the entire tree is the sum of the number of directories
        // created at each level of recursion. Therefore, the total number of directories created is:
        std::size_t total_directory_count = 0;
        for (std::size_t d = 1; d <= recursion_depth; ++d)
        {
            total_directory_count += std::pow(directories_per_depth, d);
        }
        return total_directory_count;
    }
}
}
}