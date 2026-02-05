#include <iostream>
#include <QImage>
#include <QColor>
#include <QSize>
#include <QFile>

int main()
{
    std::cout << "Testing basic photo editor functionality..." << std::endl;
    
    // Test 1: Basic QImage functionality
    QImage testImage(100, 100, QImage::Format_ARGB32);
    testImage.fill(Qt::red);
    
    if (testImage.isNull()) {
        std::cout << "FAIL: Image creation failed" << std::endl;
        return 1;
    }
    
    if (testImage.size() != QSize(100, 100)) {
        std::cout << "FAIL: Image size incorrect" << std::endl;
        return 1;
    }
    
    // Test 2: Color functionality
    QColor pixelColor = testImage.pixelColor(50, 50);
    if (pixelColor != QColor(Qt::red)) {
        std::cout << "FAIL: Image color incorrect" << std::endl;
        return 1;
    }
    
    // Test 3: Image operations
    QImage scaledImage = testImage.scaled(50, 50, Qt::KeepAspectRatio);
    if (scaledImage.isNull()) {
        std::cout << "FAIL: Image scaling failed" << std::endl;
        return 1;
    }
    
    // Test 4: File operations
    QString testFile = "test_image.png";
    if (!testImage.save(testFile)) {
        std::cout << "FAIL: Image save failed" << std::endl;
        return 1;
    }
    
    QImage loadedImage(testFile);
    if (loadedImage.isNull()) {
        std::cout << "FAIL: Image load failed" << std::endl;
        return 1;
    }
    
    // Cleanup
    QFile::remove(testFile);
    
    std::cout << "SUCCESS: All basic tests passed!" << std::endl;
    return 0;
}
