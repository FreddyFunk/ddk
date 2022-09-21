#pragma once

#include "../fsitem.hpp"
#include <unordered_map>

namespace FSI::DATA
{   
    // supported image types
	enum class ImageType
	{
        // All unsupported image file formats
        UNSUPPORTED,
        // Native OpenCV Support
		BMP, // Windows bitmaps - *.bmp, *.dib
		JPEG, // JPEG files - *.jpeg, *.jpg, *.jpe
        JPEG2000, // JPEG 2000 files - *.jp2
        PNG, // Portable Network Graphics - *.png
        WEBP, // WebP - *.webp
        PIF, // Portable image format - *.pbm, *.pgm, *.ppm *.pxm, *.pnm
        PFM, // PFM files - *.pfm
        SR, // Sun rasters - *.sr, *.ras
        TIFF, // TIFF files - *.tiff, *.tif
        EXR, // OpenEXR Image files - *.exr
        RHDR, // Radiance HDR - *.hdr, *.pic
	};

    static std::unordered_map<std::string, ImageType> const ImageTypeConversionTable = { 
            {".bmp", ImageType::BMP},
            {".dip", ImageType::BMP},
            {".jpeg", ImageType::JPEG},
            {".jpg", ImageType::JPEG},
            {".jpe", ImageType::JPEG},
            {".jp2", ImageType::JPEG2000},
            {".png", ImageType::PNG},
            {".webp", ImageType::WEBP},
            {".pbm", ImageType::PIF},
            {".pgm", ImageType::PIF},
            {".ppm", ImageType::PIF},
            {".pxm", ImageType::PIF},
            {".pnm", ImageType::PIF},
            {".pfm", ImageType::PFM},
            {".sr", ImageType::SR},
            {".ras", ImageType::SR},
            {".tiff", ImageType::TIFF},
            {".tif", ImageType::TIFF},
            {".exr", ImageType::EXR},
            {".hdr", ImageType::RHDR},
            {".pic", ImageType::RHDR}
        };


    class Image {
    public:
        Image(const FSI::FileSystemItem* const item);
        ~Image();
        ImageType getType() const;

    private:
        ImageType getFromFileExtension(const FSI::FileSystemItem* const item) const;

        const FSI::FileSystemItem* const m_item;
        const ImageType m_type;
    };
}
