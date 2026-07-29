#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <cstdint>
#include <cassert>

#pragma pack(push, 1)
struct BMPHeader {
    uint16_t bfType;
    uint32_t bfSize;
    uint16_t bfReserved1;
    uint16_t bfReserved2;
    uint32_t bfOffbits;

    uint32_t biSize;
    int32_t  biWidth;
    int32_t  biHeight;
    uint16_t biPlanes;
    uint16_t biBitCount;
    uint32_t biCompression;
    uint32_t biSizeImage;
    int32_t  biXPelsPerMeter;
    int32_t  biYPelsPerMeter;
    uint32_t biClrUsed;
    uint32_t biClrImportant;
};
#pragma pack(pop)

class BMPImage {
private:
    BMPHeader header{};
    std::vector<uint8_t> pixels;

    int getPadding() const {
        return (4 - (header.biWidth * 3) % 4) % 4;
    }

public:
    BMPImage() = default;

    bool open(const std::string& filePath) {
        std::ifstream file(filePath, std::ios::binary);
        if (!file.is_open()) {
            std::cerr << "Error opening file: " << filePath << "\n";
            return false;
        }

        file.read(reinterpret_cast<char*>(&header), sizeof(BMPHeader));

        if (!isValid()) {
            std::cerr << "Not valid BMP file!\n";
            return false;
        }

        size_t totalPixelsBytes = header.biWidth * std::abs(header.biHeight) * 3;
        pixels.resize(totalPixelsBytes);

        file.seekg(header.bfOffbits, std::ios::beg);

        int padding = getPadding();
        for (int i = 0; i < std::abs(header.biHeight); ++i) {
            file.read(reinterpret_cast<char*>(pixels.data() + (i * header.biWidth * 3)), header.biWidth * 3);
            file.seekg(padding, std::ios::cur);
        }

        return true;
    }

    bool save(const std::string& filePath) const {
        std::ofstream file(filePath, std::ios::binary);
        if (!file.is_open()) {
            std::cerr << "Error creating file: " << filePath << "\n";
            return false;
        }

        file.write(reinterpret_cast<const char*>(&header), sizeof(BMPHeader));

        int padding = getPadding();
        uint8_t padBytes[3] = {0, 0, 0};

        for (int i = 0; i < std::abs(header.biHeight); ++i) {
            file.write(reinterpret_cast<const char*>(pixels.data() + (i * header.biWidth * 3)), header.biWidth * 3);
            if (padding > 0) {
                file.write(reinterpret_cast<const char*>(padBytes), padding);
            }
        }

        return true;
    }

    void invertColors() {
        for (auto& byte : pixels) {
            byte = 255 - byte;
        }
    }

    bool isValid() const {
        return header.bfType == 0x4D42;
    }

    int32_t getWidth() const { return header.biWidth; }
    int32_t getHeight() const { return header.biHeight; }
    uint16_t getBitCount() const { return header.biBitCount; }
};

int main() {
    BMPImage img;

    std::string inPath = "ice.bmp";
    std::string outPath = "new_ice_cpp.bmp";

    if (!img.open(inPath)) {
        return 1;
    }

    std::cout << "Is BMP: " << (img.isValid() ? "Yes" : "No") << "\n";
    std::cout << "Width: " << img.getWidth() << "\n";
    std::cout << "Height: " << img.getHeight() << "\n";
    std::cout << "Bits per pixel: " << img.getBitCount() << "\n";
    assert(img.getBitCount() == 24);

    img.invertColors();
    if (img.save(outPath)) {
        std::cout << "Saved inverted image successfully!\n";
    }

    return 0;
}