#include "common.hpp"
#include "deduplication.hpp"
#include "io/mmap_file_read_only.hpp"

#include "xxh3.h"
namespace FSI::FILTER::DEDUPLICATION
{
    XXH64_hash_t hashFileStream(const std::filesystem::path& path)
    {
        // Fallback when mapped memory is not supported
        // TODO: Optimize i/o with mapped memory
        // TODO: Multithread hash calculation 
        // dual swapping (pointer switch, buffer overwrite not clearing) input buffers
        // indivial input buffers should be a multiple of bufferSize and at least bufferSize * threads

        return {};
    }

    XXH64_hash_t hashMappedMemory(const std::filesystem::path& path)
    {
        // TODO: Error Handling
        FSI::IO::MMapFileReadOnly file(path.c_str());
        XXH64_hash_t hash = XXH64(file.getData(), file.getSize(), 0);
        return hash;
    }

    void calculateHashValues(std::vector<FileSystemItem*>& items){
        for (FileSystemItem* const item : items)
		{
            item->setHash(hashMappedMemory(item->getPath()));
		}   
    }

    void removeFilesWithUniqueSize(std::vector<FileSystemItem*>& items){
        COMMON::sortFSitemsBySize(items);

        std::size_t itemsCount = items.size();
        for (std::size_t i = 0; i < itemsCount; i++){
            const bool previousDifferent = i == 0 ? true : (items.at(i)->getSizeInBytes() != items.at(i - 1)->getSizeInBytes());
            const bool nextDifferent = i == itemsCount - 1 ? true : (items.at(i)->getSizeInBytes() != items.at(i + 1)->getSizeInBytes());
            if (previousDifferent && nextDifferent){
                items.erase(items.begin() + i);
                i--;
                itemsCount--;
            }
        }
    }

    void removeFilesWithUniqueHash(std::vector<FileSystemItem*>& items){
        COMMON::sortFSitemsByHash(items);
        
        std::size_t itemsCount = items.size();
        for (std::size_t i = 0; i < itemsCount; i++){
            const bool previousDifferent = i == 0 ? true : (items.at(i)->getHash() != items.at(i - 1)->getHash());
            const bool nextDifferent = i == itemsCount - 1 ? true : (items.at(i)->getHash() != items.at(i + 1)->getHash());
            if (previousDifferent && nextDifferent){
                items.erase(items.begin() + i);
                i--;
                itemsCount--;
            }
        }
    }

    void tagDuplicateBinaries(std::vector<FileSystemItem*>& items){
        COMMON::onlyFiles(items);
        COMMON::removeEmptyFiles(items);
        removeFilesWithUniqueSize(items);
        calculateHashValues(items);
        removeFilesWithUniqueHash(items);

        for (std::size_t i = 0; i < items.size(); i++){
            std::size_t range = i;
            while (range < items.size() - 1 && items.at(i)->getHash() == items.at(range + 1)->getHash())
            {
                range++;
            }
            
            // TODO: Refactor this nasty logic
            for (std::size_t outer = 0; outer < range - i; outer++)
            {
                for (std::size_t inner = 0; inner < range - i; inner++)
                {
                    if (inner != outer)
                    {
                        items.at(i + inner)->addDuplicate(items.at(i + outer));
                        items.at(i + outer)->addDuplicate(items.at(i + inner));
                    }
                }
            }

            i += range - i;
        }
    }

    std::vector<std::vector<FileSystemItem*>> getDuplicateClusters(const std::vector<FileSystemItem*>& items){
        std::vector<FileSystemItem*> duplicates{};

        // collect all duplicates
		for (auto item : items)
		{
            duplicates.push_back(item);
            const auto& taggedDuplicates = item->getDuplicates();
            duplicates.insert(duplicates.end(), taggedDuplicates.begin(), taggedDuplicates.end());
		}

        // remove redundant duplicates
        std::sort(duplicates.begin(), duplicates.end());
        duplicates.erase(std::unique(duplicates.begin(), duplicates.end()), duplicates.end());

        COMMON::sortFSitemsByHash(duplicates);

        return COMMON::makeClusters(duplicates);
    }
}
