#include <iostream>
#include <vector>
#include <cmath>
#include <memory>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// Класс, представляющий изображение
class Image {
public:
    Image(const std::string& filename) : filename(filename) {}

    virtual std::vector<unsigned char> loadImage(int& width, int& height, int& channels) const {
        std::vector<unsigned char> image;
        stbi_uc* data = stbi_load(filename.c_str(), &width, &height, &channels, STBI_rgb);
        if (data) {
            image.assign(data, data + (width * height * channels));
            stbi_image_free(data);
        }
        return image;
    }

    virtual ~Image() = default;

private:
    std::string filename;
};

// Класс, представляющий преобразование изображения в оттенки серого
class GrayscaleConverter {
public:
    virtual std::vector<unsigned char> convertToGrayscale(const std::vector<unsigned char>& image, int width, int height) const {
        std::vector<unsigned char> grayscaleImage(width * height);

        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                int index = (y * width + x) * 3;
                unsigned char r = image[index];
                unsigned char g = image[index + 1];
                unsigned char b = image[index + 2];
                grayscaleImage[y * width + x] = static_cast<unsigned char>(0.2126 * r + 0.7152 * g + 0.0722 * b);
            }
        }

        return grayscaleImage;
    }

    virtual ~GrayscaleConverter() = default;
};

// Интерфейс для создания ASCII-арта
class AsciiArt {
public:
    virtual void createAsciiArt(const std::vector<unsigned char>& grayscaleImage, int width, int height, int asciiWidth) const = 0;

    virtual ~AsciiArt() = default;
};

// Класс, реализующий создание ASCII-арта
class BasicAsciiArt : public AsciiArt {
public:
    virtual void createAsciiArt(const std::vector<unsigned char>& grayscaleImage, int width, int height, int asciiWidth) const override {
        const char grayscale[] = "@%#*+=-:. ";

        int asciiHeight = height * asciiWidth / (width * 2); // Пропорциональное изменение высоты ASCII-арта

        for (int y = 0; y < asciiHeight; ++y) {
            for (int x = 0; x < asciiWidth; ++x) {
                int imgX = x * width / asciiWidth;
                int imgY = y * height / asciiHeight;
                int pixelValue = grayscaleImage[imgY * width + imgX];
                int index = pixelValue * 10 / 255; // Преобразование значения пикселя к градации серого
                std::cout << grayscale[index];
            }
            std::cout << std::endl;
        }
    }
};

// Фабрика для создания объектов, отвечающих за обработку изображений и создание ASCII-арта
class ImageProcessorFactory {
public:
    static std::shared_ptr<Image> createImage(const std::string& filename) {
        return std::make_shared<Image>(filename);
    }

    static std::shared_ptr<GrayscaleConverter> createGrayscaleConverter() {
        return std::make_shared<GrayscaleConverter>();
    }

    static std::shared_ptr<AsciiArt> createAsciiArt() {
        return std::make_shared<BasicAsciiArt>();
    }
};

int main() {
    std::string filename;
    int asciiWidth;

    do {
        std::cout << "Enter the path to the image file (or 'q' to quit): ";
        std::cin >> filename;
        if (filename == "q") {
            break;
        }

        std::shared_ptr<Image> image = ImageProcessorFactory::createImage(filename);
        int width, height, channels;
        std::vector<unsigned char> imageData = image->loadImage(width, height, channels);
        if (imageData.empty()) {
            std::cerr << "Failed to load image!" << std::endl;
            continue;
        }

        std::cout << "Enter the desired width for the ASCII art: ";
        std::cin >> asciiWidth;

        std::shared_ptr<GrayscaleConverter> converter = ImageProcessorFactory::createGrayscaleConverter();
        std::vector<unsigned char> grayscaleImage = converter->convertToGrayscale(imageData, width, height);

        std::shared_ptr<AsciiArt> asciiArt = ImageProcessorFactory::createAsciiArt();
        std::cout << "Converting grayscale image to ASCII art (width = " << asciiWidth << "):" << std::endl;
        asciiArt->createAsciiArt(grayscaleImage, width, height, asciiWidth);

        std::cout << std::endl;
    } while (true);

    return 0;
}