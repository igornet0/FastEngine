#pragma once

#include <string>
#include <vector>

namespace FastEngine {
    enum class ImageFormat {
        BMP,
        PNG,
        JPG,
        TGA,
        UNKNOWN
    };
    
    struct ImageData {
        unsigned char* data;
        int width;
        int height;
        int channels;
        ImageFormat format;
        
        ImageData() : data(nullptr), width(0), height(0), channels(0), format(ImageFormat::UNKNOWN) {}
        ~ImageData() { delete[] data; }
        
        // Запрещаем копирование
        ImageData(const ImageData&) = delete;
        ImageData& operator=(const ImageData&) = delete;
        
        // Разрешаем перемещение
        ImageData(ImageData&& other) noexcept 
            : data(other.data), width(other.width), height(other.height), 
              channels(other.channels), format(other.format) {
            other.data = nullptr;
        }
        
        ImageData& operator=(ImageData&& other) noexcept {
            if (this != &other) {
                delete[] data;
                data = other.data;
                width = other.width;
                height = other.height;
                channels = other.channels;
                format = other.format;
                other.data = nullptr;
            }
            return *this;
        }
    };
    
    class ImageLoader {
    public:
        // Определение формата по расширению файла
        static ImageFormat GetFormatFromExtension(const std::string& filePath);
        
        // Загрузка изображения из файла
        static ImageData LoadFromFile(const std::string& filePath);
        
        // Загрузка изображения из памяти
        static ImageData LoadFromMemory(const std::vector<unsigned char>& data, ImageFormat format);
        
        // Создание цветной текстуры
        static ImageData CreateColorTexture(int width, int height, unsigned char r, unsigned char g, unsigned char b, unsigned char a = 255);
        
        // Создание текстуры шахматной доски для отладки
        static ImageData CreateCheckerboard(int width, int height, int checkerSize = 32);
        
    private:
        // Загрузчики для разных форматов
        static ImageData LoadBMP(const std::string& filePath);
        static ImageData LoadPNG(const std::string& filePath);
        static ImageData LoadJPG(const std::string& filePath);
        static ImageData LoadTGA(const std::string& filePath);
        
        // Загрузка из памяти
        static ImageData LoadBMPFromMemory(const std::vector<unsigned char>& data);
        static ImageData LoadPNGFromMemory(const std::vector<unsigned char>& data);
        static ImageData LoadJPGFromMemory(const std::vector<unsigned char>& data);
        static ImageData LoadTGAFromMemory(const std::vector<unsigned char>& data);
    };
}

