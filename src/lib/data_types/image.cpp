#include "image.hpp"

namespace FSI::DATA
{
    Image::Image(const FSI::FileSystemItem* const item)
    : m_item(item), m_type(getFromFileExtension(item))
    {
        
    }
    Image::~Image() {

    }

    ImageType Image::getType() const {
        return m_type;
    }

    ImageType Image::getFromFileExtension(const FSI::FileSystemItem* const item) const {
        if (!item->getPath().has_extension())
        {
            return ImageType::UNSUPPORTED;
        }

        std::string extension = item->getPath().extension();
        std::transform(extension.begin(), extension.end(), extension.begin(),
            [](unsigned char c){ return std::tolower(c); });
        
        auto it = ImageTypeConversionTable.find(extension);
        if (it != ImageTypeConversionTable.end()) {
            return it->second;
        } 
        else {
            return ImageType::UNSUPPORTED;
        }
    }
}
