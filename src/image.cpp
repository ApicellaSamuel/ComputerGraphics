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
#include "math.h"
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

    auto pixelsl = stbi_loadf(filename.c_str(), &w, &h, &c, 0);

    if (!pixelsl) return {};

    auto img = image4f(w, h);
    int p = 0;
    for(int j = 0; j < h; j++){
        for(int i = 0; i < w; i++){
            img.at(i, j).x = pixelsl[p++];
            img.at(i, j).y = pixelsl[p++];
            img.at(i, j).z = pixelsl[p++];
            img.at(i, j).w = pixelsl[p++];
        }
    }

    return img;
}

image4b load_image4b(const std::string& filename) {
    auto w = 0, h = 0, c = 0;
    auto pixelsl = stbi_load(filename.c_str(), &w, &h, &c, 4);

    if (!pixelsl) return {};

    auto img = image4b(w, h);
    int p = 0;
        for(int j = 0; j < h; j++){
            for(int i = 0; i < w; i++){
                img.at(i, j).x = pixelsl[p++];
                img.at(i, j).y = pixelsl[p++];
                img.at(i, j).z = pixelsl[p++];
                img.at(i, j).w = pixelsl[p++];
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
    for(int i = 0; i < hdr.width; i++){
        for(int j = 0; j < hdr.height; j++){
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
    auto img = image4b(imgs[0].width, imgs[0].height);
    if(premultiplied){ cout << "premultiplied\n";}
    else{ cout << "no premultiplied\n";}
    if(no_srgb){ cout << "nosrgb\n";}
    else{ cout << "srgb\n";}
    for(int i = 0; i < img.width; i++){
        for(int j = 0; j < img.height; j++){
            //primo metodo per il caso in cui i valori non sono premoltiplicati
            img.at(i, j).x += pow((  imgs[1].at(i, j).x * imgs[1].at(i, j).w  + ( 1 - imgs[1].at(i, j).w) * (imgs[0].at(i, j).w * imgs[0].at(i, j).x)  )*255.0f, 2.2f);
            img.at(i, j).y += pow((  imgs[1].at(i, j).y * imgs[1].at(i, j).w  + ( 1 - imgs[1].at(i, j).w) * (imgs[0].at(i, j).w * imgs[0].at(i, j).y)  )*255.0f, 2.2f);
            img.at(i, j).z += pow((  imgs[1].at(i, j).z * imgs[1].at(i, j).w  + ( 1 - imgs[1].at(i, j).w) * (imgs[0].at(i, j).w * imgs[0].at(i, j).z)  )*255.0f, 2.2f);
            img.at(i, j).w +=(  imgs[1].at(i, j).w + ( 1 - imgs[1].at(i, j).w) * imgs[0].at(i, j).w  )/255.0f;
            //secondo metodo per valori premoltiplicati
            /*img.at(i, j).x += (pow((imgs[1].at(i, j).x/255.0f), 2.2f)  +  ( 1 - imgs[1].at(i, j).w/255.0f )  *  pow((imgs[0].at(i, j).x/255.0f), 2.2f));
            img.at(i, j).y += (pow((imgs[1].at(i, j).y/255.0f), 2.2f)  +  ( 1 - imgs[1].at(i, j).w/255.0f )  *  pow((imgs[0].at(i, j).y/255.0f), 2.2f));
            img.at(i, j).z += (pow((imgs[1].at(i, j).z/255.0f), 2.2f)  +  ( 1 - imgs[1].at(i, j).w/255.0f )  *  pow((imgs[0].at(i, j).z/255.0f), 2.2f));
            img.at(i, j).w += ((imgs[1].at(i, j).w/255.0f)  +  ( 1 - imgs[1].at(i, j).w/255.0f )  *  imgs[0].at(i, j).w/255.0f);*/
            /*img.at(i, j).x += (imgs[1].at(i, j).x/255.0f)  +  ( 1 - imgs[1].at(i, j).w/255.0f )  *  (imgs[0].at(i, j).x/255.0f);
            img.at(i, j).y += (imgs[1].at(i, j).y/255.0f)  +  ( 1 - imgs[1].at(i, j).w/255.0f )  *  (imgs[0].at(i, j).y/255.0f);
            img.at(i, j).z += (imgs[1].at(i, j).z/255.0f)  +  ( 1 - imgs[1].at(i, j).w/255.0f )  *  (imgs[0].at(i, j).z/255.0f);
            img.at(i, j).w += (imgs[1].at(i, j).w/255.0f)  +  ( 1 - imgs[1].at(i, j).w/255.0f )  *  imgs[0].at(i, j).w/255.0f;*/

        }
    }
    return img;
}
