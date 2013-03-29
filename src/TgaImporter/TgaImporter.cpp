/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013 Vladimír Vondruš <mosra@centrum.cz>

    Permission is hereby granted, free of charge, to any person obtaining a
    copy of this software and associated documentation files (the "Software"),
    to deal in the Software without restriction, including without limitation
    the rights to use, copy, modify, merge, publish, distribute, sublicense,
    and/or sell copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included
    in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
    THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.
*/

#include "TgaImporter.h"

#include <fstream>
#include <sstream>
#include <algorithm>
#include <Utility/Endianness.h>
#include <Math/Vector2.h>
#include <Swizzle.h>
#include <Trade/ImageData.h>

using Corrade::Utility::Endianness;

namespace Magnum { namespace Trade { namespace TgaImporter {

#ifndef DOXYGEN_GENERATING_OUTPUT
static_assert(sizeof(TgaImporter::Header) == 18, "TgaImporter: header size is not 18 bytes");
#endif

TgaImporter::TgaImporter(): in(nullptr) {}

TgaImporter::TgaImporter(Corrade::PluginManager::AbstractPluginManager* manager, std::string plugin): AbstractImporter(manager, std::move(plugin)), in(nullptr) {}

TgaImporter::~TgaImporter() { close(); }

TgaImporter::Features TgaImporter::features() const {
    return Feature::OpenData|Feature::OpenFile;
}

bool TgaImporter::TgaImporter::openData(const void* const data, const std::size_t size) {
    close();

    in = new std::istringstream(std::string(reinterpret_cast<const char*>(data), size));
    return true;
}

bool TgaImporter::TgaImporter::openFile(const std::string& filename) {
    close();

    in = new std::ifstream(filename.c_str());
    if(in->good()) return true;

    Error() << "TgaImporter: cannot open file" << filename;
    close();
    return false;
}

void TgaImporter::close() {
    delete in;
    in = nullptr;
}

UnsignedInt TgaImporter::TgaImporter::image2DCount() const {
    return in ? 1 : 0;
}

ImageData2D* TgaImporter::image2D(UnsignedInt id) {
    CORRADE_ASSERT(in, "Trade::TgaImporter::TgaImporter::image2D(): no file opened", nullptr);
    CORRADE_ASSERT(id == 0, "Trade::TgaImporter::TgaImporter::image2D(): wrong image ID", nullptr);

    /* Check if the file is long enough */
    in->seekg(0, std::istream::end);
    std::streampos filesize = in->tellg();
    in->seekg(0, std::istream::beg);
    if(filesize < std::streampos(sizeof(Header))) {
        Error() << "TgaImporter: the file is too short:" << filesize << "bytes";
        return nullptr;
    }

    Header header;
    in->read(reinterpret_cast<char*>(&header), sizeof(Header));

    /* Convert to machine endian */
    header.width = Endianness::littleEndian(header.width);
    header.height = Endianness::littleEndian(header.height);

    if(header.colorMapType != 0) {
        Error() << "TgaImporter: paletted files are not supported";
        return nullptr;
    }

    if(header.imageType != 2) {
        Error() << "TgaImporter: non-RGB files are not supported";
        return nullptr;
    }

    ImageData2D::Format format;
    switch(header.bpp) {
        case 24:
            #ifndef MAGNUM_TARGET_GLES
            format = ImageData2D::Format::BGR;
            #else
            format = ImageData2D::Format::RGB;
            #endif
            break;
        case 32:
            #ifndef MAGNUM_TARGET_GLES
            format = ImageData2D::Format::BGRA;
            #else
            format = ImageData2D::Format::RGBA;
            #endif
            break;
        default:
            Error() << "TgaImporter: unsupported bits-per-pixel:" << header.bpp;
            return nullptr;
    }

    std::size_t size = header.width*header.height*header.bpp/8;
    char* buffer = new char[size];
    in->read(buffer, size);

    Vector2i dimensions(header.width, header.height);

    #ifdef MAGNUM_TARGET_GLES
    if(format == ImageData2D::Format::RGB) {
        auto data = reinterpret_cast<Math::Vector3<UnsignedByte>*>(buffer);
        std::transform(data, data + dimensions.product(), data,
            [](Math::Vector3<UnsignedByte> pixel) { return swizzle<'b', 'g', 'r'>(pixel); });
    } else if(format == ImageData2D::Format::RGBA) {
        auto data = reinterpret_cast<Math::Vector4<UnsignedByte>*>(buffer);
        std::transform(data, data + dimensions.product(), data,
            [](Math::Vector4<UnsignedByte> pixel) { return swizzle<'b', 'g', 'r', 'a'>(pixel); });
    }
    #endif

    return new ImageData2D(dimensions, format, ImageData2D::Type::UnsignedByte, buffer);
}

}}}
