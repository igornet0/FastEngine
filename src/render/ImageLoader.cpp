#include "FastEngine/Render/ImageLoader.h"
#include "FastEngine/Platform/Platform.h"
#include "FastEngine/Platform/FileSystem.h"

#include <iostream>
#include <fstream>
#include <algorithm>
#include <cstring>

namespace FastEngine {
    ImageFormat ImageLoader::GetFormatFromExtension(const std::string& filePath) {
        std::string lowerPath = filePath;
        std::transform(lowerPath.begin(), lowerPath.end(), lowerPath.begin(), ::tolower);
        
        if (lowerPath.length() >= 4 && lowerPath.substr(lowerPath.length() - 4) == ".bmp") {
            return ImageFormat::BMP;
        } else if (lowerPath.length() >= 4 && lowerPath.substr(lowerPath.length() - 4) == ".png") {
            return ImageFormat::PNG;
        } else if ((lowerPath.length() >= 4 && lowerPath.substr(lowerPath.length() - 4) == ".jpg") ||
                   (lowerPath.length() >= 5 && lowerPath.substr(lowerPath.length() - 5) == ".jpeg")) {
            return ImageFormat::JPG;
        } else if (lowerPath.length() >= 4 && lowerPath.substr(lowerPath.length() - 4) == ".tga") {
            return ImageFormat::TGA;
        }
        
        return ImageFormat::UNKNOWN;
    }
    
    ImageData ImageLoader::LoadFromFile(const std::string& filePath) {
        // Сначала пробуем прочитать через FileSystem
        auto* fileSystem = Platform::GetInstance().GetFileSystem();
        std::string fullPath = fileSystem ? fileSystem->GetResourcePath(filePath) : filePath;
        
        ImageFormat format = GetFormatFromExtension(filePath);
        
        switch (format) {
            case ImageFormat::BMP:
                return LoadBMP(fullPath);
            case ImageFormat::PNG:
                return LoadPNG(fullPath);
            case ImageFormat::JPG:
                return LoadJPG(fullPath);
            case ImageFormat::TGA:
                return LoadTGA(fullPath);
            default:
                std::cerr << "Unsupported image format: " << filePath << std::endl;
                return ImageData();
        }
    }
    
    ImageData ImageLoader::LoadFromMemory(const std::vector<unsigned char>& data, ImageFormat format) {
        switch (format) {
            case ImageFormat::BMP:
                return LoadBMPFromMemory(data);
            case ImageFormat::PNG:
                return LoadPNGFromMemory(data);
            case ImageFormat::JPG:
                return LoadJPGFromMemory(data);
            case ImageFormat::TGA:
                return LoadTGAFromMemory(data);
            default:
                std::cerr << "Unsupported image format for memory loading" << std::endl;
                return ImageData();
        }
    }
    
    ImageData ImageLoader::CreateColorTexture(int width, int height, unsigned char r, unsigned char g, unsigned char b, unsigned char a) {
        ImageData image;
        image.width = width;
        image.height = height;
        image.channels = 4; // RGBA
        image.format = ImageFormat::UNKNOWN; // Синтетическая текстура
        
        int dataSize = width * height * image.channels;
        image.data = new unsigned char[dataSize];
        
        for (int i = 0; i < dataSize; i += 4) {
            image.data[i + 0] = r; // R
            image.data[i + 1] = g; // G
            image.data[i + 2] = b; // B
            image.data[i + 3] = a; // A
        }
        
        return image;
    }
    
    ImageData ImageLoader::CreateCheckerboard(int width, int height, int checkerSize) {
        ImageData image;
        image.width = width;
        image.height = height;
        image.channels = 4; // RGBA
        image.format = ImageFormat::UNKNOWN; // Синтетическая текстура
        
        int dataSize = width * height * image.channels;
        image.data = new unsigned char[dataSize];
        
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                int index = (y * width + x) * 4;
                
                // Определяем цвет на основе позиции
                bool isWhite = ((x / checkerSize) + (y / checkerSize)) % 2 == 0;
                
                if (isWhite) {
                    image.data[index + 0] = 255; // R
                    image.data[index + 1] = 255; // G
                    image.data[index + 2] = 255; // B
                    image.data[index + 3] = 255; // A
                } else {
                    image.data[index + 0] = 128; // R
                    image.data[index + 1] = 128; // G
                    image.data[index + 2] = 128; // B
                    image.data[index + 3] = 255; // A
                }
            }
        }
        
        return image;
    }
    
    // BMP Loader
    ImageData ImageLoader::LoadBMP(const std::string& filePath) {
        ImageData image;
        std::ifstream file(filePath, std::ios::binary);
        if (!file.is_open()) {
            return image;
        }
        
        // Читаем заголовок BMP
        char header[54];
        file.read(header, 54);
        
        // Проверяем сигнатуру BMP
        if (header[0] != 'B' || header[1] != 'M') {
            return image;
        }
        
        // Извлекаем размеры
        image.width = *(int*)&header[18];
        image.height = *(int*)&header[22];
        image.channels = 3; // BMP обычно RGB
        image.format = ImageFormat::BMP;
        
        // Читаем данные изображения
        int dataSize = image.width * image.height * image.channels;
        image.data = new unsigned char[dataSize];
        
        // Читаем данные из файла
        file.seekg(54); // Пропускаем заголовок
        file.read(reinterpret_cast<char*>(image.data), dataSize);
        
        // BMP хранится в BGR формате, нужно конвертировать в RGB
        for (int i = 0; i < dataSize; i += image.channels) {
            // Конвертируем BGR в RGB
            unsigned char temp = image.data[i];
            image.data[i] = image.data[i + 2];     // R
            image.data[i + 2] = temp;              // B
            // G остается на месте
        }
        
        return image;
    }
    
    ImageData ImageLoader::LoadBMPFromMemory(const std::vector<unsigned char>& data) {
        ImageData image;
        if (data.size() < 54) {
            return image;
        }
        
        // Проверяем сигнатуру BMP
        if (data[0] != 'B' || data[1] != 'M') {
            return image;
        }
        
        // Извлекаем размеры
        image.width = *(int*)&data[18];
        image.height = *(int*)&data[22];
        image.channels = 3; // BMP обычно RGB
        image.format = ImageFormat::BMP;
        
        // Читаем данные изображения
        int dataSize = image.width * image.height * image.channels;
        if (data.size() < 54 + dataSize) {
            return image;
        }
        
        image.data = new unsigned char[dataSize];
        memcpy(image.data, data.data() + 54, dataSize);
        
        // BMP хранится в BGR формате, нужно конвертировать в RGB
        for (int i = 0; i < dataSize; i += image.channels) {
            // Конвертируем BGR в RGB
            unsigned char temp = image.data[i];
            image.data[i] = image.data[i + 2];     // R
            image.data[i + 2] = temp;              // B
            // G остается на месте
        }
        
        return image;
    }
    
    // PNG Loader (простая реализация)
    ImageData ImageLoader::LoadPNG(const std::string& filePath) {
        ImageData image;
        std::ifstream file(filePath, std::ios::binary);
        if (!file.is_open()) {
            return image;
        }
        
        // Читаем весь файл
        file.seekg(0, std::ios::end);
        size_t fileSize = file.tellg();
        file.seekg(0, std::ios::beg);
        
        std::vector<unsigned char> buffer(fileSize);
        file.read(reinterpret_cast<char*>(buffer.data()), fileSize);
        
        return LoadPNGFromMemory(buffer);
    }
    
    ImageData ImageLoader::LoadPNGFromMemory(const std::vector<unsigned char>& data) {
        ImageData image;
        
        // Простая проверка PNG сигнатуры
        if (data.size() < 8) {
            return image;
        }
        
        // PNG сигнатура: 89 50 4E 47 0D 0A 1A 0A
        if (data[0] != 0x89 || data[1] != 0x50 || data[2] != 0x4E || data[3] != 0x47 ||
            data[4] != 0x0D || data[5] != 0x0A || data[6] != 0x1A || data[7] != 0x0A) {
            return image;
        }
        
        // Для простоты создаем заглушку PNG
        // В реальном проекте здесь должна быть полная реализация PNG декодера
        // или использование библиотеки типа stb_image
        image.width = 64;
        image.height = 64;
        image.channels = 4;
        image.format = ImageFormat::PNG;
        
        int dataSize = image.width * image.height * image.channels;
        image.data = new unsigned char[dataSize];
        
        // Создаем простую текстуру с градиентом
        for (int y = 0; y < image.height; y++) {
            for (int x = 0; x < image.width; x++) {
                int index = (y * image.width + x) * 4;
                image.data[index + 0] = (x * 255) / image.width;     // R
                image.data[index + 1] = (y * 255) / image.height;    // G
                image.data[index + 2] = 128;                         // B
                image.data[index + 3] = 255;                         // A
            }
        }
        
        return image;
    }
    
    // JPG Loader (простая реализация)
    ImageData ImageLoader::LoadJPG(const std::string& filePath) {
        ImageData image;
        std::ifstream file(filePath, std::ios::binary);
        if (!file.is_open()) {
            return image;
        }
        
        // Читаем весь файл
        file.seekg(0, std::ios::end);
        size_t fileSize = file.tellg();
        file.seekg(0, std::ios::beg);
        
        std::vector<unsigned char> buffer(fileSize);
        file.read(reinterpret_cast<char*>(buffer.data()), fileSize);
        
        return LoadJPGFromMemory(buffer);
    }
    
    ImageData ImageLoader::LoadJPGFromMemory(const std::vector<unsigned char>& data) {
        ImageData image;
        
        // Простая проверка JPG сигнатуры
        if (data.size() < 2) {
            return image;
        }
        
        // JPG сигнатура: FF D8
        if (data[0] != 0xFF || data[1] != 0xD8) {
            return image;
        }
        
        // Для простоты создаем заглушку JPG
        // В реальном проекте здесь должна быть полная реализация JPG декодера
        image.width = 64;
        image.height = 64;
        image.channels = 3;
        image.format = ImageFormat::JPG;
        
        int dataSize = image.width * image.height * image.channels;
        image.data = new unsigned char[dataSize];
        
        // Создаем простую текстуру с градиентом
        for (int y = 0; y < image.height; y++) {
            for (int x = 0; x < image.width; x++) {
                int index = (y * image.width + x) * 3;
                image.data[index + 0] = (x * 255) / image.width;     // R
                image.data[index + 1] = (y * 255) / image.height;    // G
                image.data[index + 2] = 128;                         // B
            }
        }
        
        return image;
    }
    
    // TGA Loader (простая реализация)
    ImageData ImageLoader::LoadTGA(const std::string& filePath) {
        ImageData image;
        std::ifstream file(filePath, std::ios::binary);
        if (!file.is_open()) {
            return image;
        }
        
        // Читаем весь файл
        file.seekg(0, std::ios::end);
        size_t fileSize = file.tellg();
        file.seekg(0, std::ios::beg);
        
        std::vector<unsigned char> buffer(fileSize);
        file.read(reinterpret_cast<char*>(buffer.data()), fileSize);
        
        return LoadTGAFromMemory(buffer);
    }
    
    ImageData ImageLoader::LoadTGAFromMemory(const std::vector<unsigned char>& data) {
        ImageData image;
        
        if (data.size() < 18) {
            return image;
        }
        
        // Читаем заголовок TGA
        image.width = data[12] | (data[13] << 8);
        image.height = data[14] | (data[15] << 8);
        int bpp = data[16];
        
        if (bpp == 24) {
            image.channels = 3;
        } else if (bpp == 32) {
            image.channels = 4;
        } else {
            return image; // Неподдерживаемый формат
        }
        
        image.format = ImageFormat::TGA;
        
        int dataSize = image.width * image.height * image.channels;
        if (data.size() < 18 + dataSize) {
            return image;
        }
        
        image.data = new unsigned char[dataSize];
        memcpy(image.data, data.data() + 18, dataSize);
        
        // TGA хранится в BGR формате, конвертируем в RGB
        for (int i = 0; i < dataSize; i += image.channels) {
            unsigned char temp = image.data[i];
            image.data[i] = image.data[i + 2];     // R
            image.data[i + 2] = temp;              // B
            // G остается на месте
        }
        
        return image;
    }
}
