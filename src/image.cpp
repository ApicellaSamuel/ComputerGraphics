//
// LICENSE:
//
// Copyright (c) 2016 -- 2017 Fabio Pellacini
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//

#include "image.h"

// needed for image loading
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// needed for image writing
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include <memory> // include this to use std::unique_ptr
#include <iostream>

using namespace std;

image4f load_image4f(const std::string& filename) {

    auto w = 0, h = 0, c = 0;
    //auto pixels = std::unique_ptr<float>(nullptr);

    auto pixelsl = std::unique_ptr<float>(stbi_loadf(filename.c_str(), &w, &h, &c, 4));

    if (!pixelsl) return {};

    float* vec = pixelsl.get();

    auto img = image4f(w, h);
    for(int i = 0; i < w; i++){
        for(int j = 0; j < h; j++){
            img.at(i, j).x = vec[j * w + i];
            img.at(i, j).y = vec[j * w + i+1];
            img.at(i, j).z = vec[j * w + i+2];
            img.at(i, j).w = vec[j * w + i+3];
        }
    }

    return img;
}

image4b load_image4b(const std::string& filename) {
    auto w = 0, h = 0, c = 0;
    auto pixelsl =
        std::unique_ptr<unsigned char>(stbi_load(filename.c_str(), &w, &h, &c, 4));
    if (!pixelsl) return {};

    unsigned char* vec = pixelsl.get();

    auto img = image4b(w, h);
    for(int i = 0; i < w; i++){
        for(int j = 0; j < h; j++){
            img.at(i, j).x = vec[j * w + i];
            img.at(i, j).y = vec[j * w + i+1];
            img.at(i, j).z = vec[j * w + i+2];
            img.at(i, j).w = vec[j * w + i+3];
        }
    }

    return img;

}

void save_image(const std::string& filename, const image4f& img) {
    stbi_write_hdr(
            filename.c_str(), img.width, img.height, 4, reinterpret_cast<const float *>(img.pixels.data()));
}

void save_image(const std::string& filename, const image4b& img) {
    stbi_write_png(filename.c_str(), img.width, img.height, 4,
                   img.pixels.data(), img.width * 4);
}


image4b tonemap(const image4f& hdr, float exposure, bool use_filmic, bool no_srgb) {
    auto img = image4b(hdr.width, hdr.height);
    for(int j = 0; j < hdr.height; j++){
        for(int i = 0; i < hdr.width; i++){
            img.at(i, j).x = (unsigned char)hdr.at(i, j).x;
            img.at(i, j).y = (unsigned char)hdr.at(i, j).y;
            img.at(i, j).z = (unsigned char)hdr.at(i, j).z;
            img.at(i, j).w = (unsigned char)hdr.at(i, j).w;
       }
    }

    return img;
}

image4b compose(
    const std::vector<image4b>& imgs, bool premultiplied, bool no_srgb) {
    // YOUR CODE GOES HERE -----------------------------------
    return imgs[0];
}
