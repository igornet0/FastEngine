#include <gtest/gtest.h>
#include <memory>
#include <vector>
#include <chrono>
#include <thread>
#include <fstream>
#include <algorithm>
#include <map>
#include <unordered_map>

// Базовые тесты для проверки эффективности и поиска багов
// без зависимости от заголовочных файлов FastEngine

class EngineBasicTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Инициализация для каждого теста
    }
    
    void TearDown() override {
        // Очистка после каждого теста
    }
};

// Тест производительности создания объектов
TEST_F(EngineBasicTest, ObjectCreationPerformance) {
    const int objectCount = 10000;
    std::vector<std::unique_ptr<int>> objects;
    
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < objectCount; ++i) {
        objects.push_back(std::make_unique<int>(i));
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    // Создание 10000 объектов должно занять менее 10мс
    EXPECT_LT(duration.count(), 10000);
    EXPECT_EQ(objects.size(), objectCount);
}

// Тест производительности математических операций
TEST_F(EngineBasicTest, MathOperationsPerformance) {
    const int iterations = 1000000;
    float result = 0.0f;
    
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < iterations; ++i) {
        result += std::sin(i * 0.001f) * std::cos(i * 0.001f);
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    // 1 миллион математических операций должно занять менее 100мс
    EXPECT_LT(duration.count(), 100);
    EXPECT_FALSE(std::isnan(result));
}

// Тест производительности работы с памятью
TEST_F(EngineBasicTest, MemoryAllocationPerformance) {
    const int allocationCount = 1000;
    const size_t allocationSize = 1024; // 1KB
    
    auto start = std::chrono::high_resolution_clock::now();
    
    std::vector<std::unique_ptr<char[]>> allocations;
    for (int i = 0; i < allocationCount; ++i) {
        allocations.push_back(std::make_unique<char[]>(allocationSize));
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    // Выделение 1000 блоков по 1KB должно занять менее 5мс
    EXPECT_LT(duration.count(), 5000);
    EXPECT_EQ(allocations.size(), allocationCount);
}

// Тест производительности работы с векторами
TEST_F(EngineBasicTest, VectorOperationsPerformance) {
    const int vectorSize = 100000;
    std::vector<float> vec1(vectorSize);
    std::vector<float> vec2(vectorSize);
    std::vector<float> result(vectorSize);
    
    // Инициализируем векторы
    for (int i = 0; i < vectorSize; ++i) {
        vec1[i] = i * 0.001f;
        vec2[i] = (i + 1) * 0.001f;
    }
    
    auto start = std::chrono::high_resolution_clock::now();
    
    // Выполняем операции с векторами
    for (int i = 0; i < vectorSize; ++i) {
        result[i] = vec1[i] + vec2[i] * 2.0f;
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    // Операции с векторами должны занять менее 10мс
    EXPECT_LT(duration.count(), 10000);
}

// Тест производительности работы с строками
TEST_F(EngineBasicTest, StringOperationsPerformance) {
    const int stringCount = 10000;
    std::vector<std::string> strings;
    
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < stringCount; ++i) {
        strings.push_back("TestString_" + std::to_string(i));
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    // Создание 10000 строк должно занять менее 50мс
    EXPECT_LT(duration.count(), 50);
    EXPECT_EQ(strings.size(), stringCount);
}

// Тест производительности многопоточности
TEST_F(EngineBasicTest, MultithreadingPerformance) {
    const int threadCount = 4;
    const int workPerThread = 25000;
    std::vector<std::thread> threads;
    std::vector<int> results(threadCount, 0);
    
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int t = 0; t < threadCount; ++t) {
        threads.emplace_back([&results, t, workPerThread]() {
            for (int i = 0; i < workPerThread; ++i) {
                results[t] += i;
            }
        });
    }
    
    for (auto& thread : threads) {
        thread.join();
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    // Многопоточная работа должна занять менее 100мс
    EXPECT_LT(duration.count(), 100);
    
    // Проверяем, что все потоки выполнили работу
    for (int i = 0; i < threadCount; ++i) {
        EXPECT_GT(results[i], 0);
    }
}

// Тест производительности работы с файлами
TEST_F(EngineBasicTest, FileOperationsPerformance) {
    const int fileCount = 100;
    const std::string testContent = "Test content for performance testing";
    
    auto start = std::chrono::high_resolution_clock::now();
    
    // Создаем временные файлы
    std::vector<std::string> filenames;
    for (int i = 0; i < fileCount; ++i) {
        std::string filename = "/tmp/test_file_" + std::to_string(i) + ".tmp";
        filenames.push_back(filename);
        
        std::ofstream file(filename);
        file << testContent;
        file.close();
    }
    
    // Читаем файлы
    for (const auto& filename : filenames) {
        std::ifstream file(filename);
        std::string content;
        std::getline(file, content);
        file.close();
    }
    
    // Удаляем файлы
    for (const auto& filename : filenames) {
        std::remove(filename.c_str());
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    // Операции с файлами должны занять менее 500мс
    EXPECT_LT(duration.count(), 500);
}

// Тест производительности работы с контейнерами
TEST_F(EngineBasicTest, ContainerOperationsPerformance) {
    const int containerSize = 50000;
    std::vector<int> vec;
    std::map<int, int> map;
    std::unordered_map<int, int> unordered_map;
    
    auto start = std::chrono::high_resolution_clock::now();
    
    // Заполняем контейнеры
    for (int i = 0; i < containerSize; ++i) {
        vec.push_back(i);
        map[i] = i * 2;
        unordered_map[i] = i * 3;
    }
    
    // Ищем элементы
    int found = 0;
    for (int i = 0; i < containerSize; i += 100) {
        if (std::find(vec.begin(), vec.end(), i) != vec.end()) found++;
        if (map.find(i) != map.end()) found++;
        if (unordered_map.find(i) != unordered_map.end()) found++;
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    // Операции с контейнерами должны занять менее 100мс
    EXPECT_LT(duration.count(), 100);
    EXPECT_GT(found, 0);
}

// Тест производительности работы с указателями
TEST_F(EngineBasicTest, PointerOperationsPerformance) {
    const int pointerCount = 100000;
    std::vector<std::unique_ptr<int>> pointers;
    
    auto start = std::chrono::high_resolution_clock::now();
    
    // Создаем указатели
    for (int i = 0; i < pointerCount; ++i) {
        pointers.push_back(std::make_unique<int>(i));
    }
    
    // Работаем с указателями
    int sum = 0;
    for (const auto& ptr : pointers) {
        sum += *ptr;
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    // Операции с указателями должны занять менее 5мс
    EXPECT_LT(duration.count(), 5000);
    EXPECT_GT(sum, 0);
}

// Тест производительности работы с функциями
TEST_F(EngineBasicTest, FunctionCallPerformance) {
    const int callCount = 1000000;
    
    auto testFunction = [](int x) -> int {
        return x * x + x;
    };
    
    auto start = std::chrono::high_resolution_clock::now();
    
    int result = 0;
    for (int i = 0; i < callCount; ++i) {
        result += testFunction(i);
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    // 1 миллион вызовов функций должно занять менее 50мс
    EXPECT_LT(duration.count(), 50);
    EXPECT_GT(result, 0);
}

// Тест производительности работы с классами
TEST_F(EngineBasicTest, ClassOperationsPerformance) {
    class TestClass {
    public:
        TestClass(int value) : m_value(value) {}
        int getValue() const { return m_value; }
        void setValue(int value) { m_value = value; }
        int calculate() const { return m_value * m_value + m_value; }
    private:
        int m_value;
    };
    
    const int classCount = 50000;
    std::vector<std::unique_ptr<TestClass>> objects;
    
    auto start = std::chrono::high_resolution_clock::now();
    
    // Создаем объекты
    for (int i = 0; i < classCount; ++i) {
        objects.push_back(std::make_unique<TestClass>(i));
    }
    
    // Работаем с объектами
    int sum = 0;
    for (const auto& obj : objects) {
        sum += obj->calculate();
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    // Операции с классами должны занять менее 50мс
    EXPECT_LT(duration.count(), 50);
    EXPECT_GT(sum, 0);
}

// Тест производительности работы с алгоритмами
TEST_F(EngineBasicTest, AlgorithmPerformance) {
    const int dataSize = 100000;
    std::vector<int> data(dataSize);
    
    // Инициализируем данные
    for (int i = 0; i < dataSize; ++i) {
        data[i] = i % 1000;
    }
    
    auto start = std::chrono::high_resolution_clock::now();
    
    // Выполняем алгоритмы
    std::sort(data.begin(), data.end());
    auto maxElement = *std::max_element(data.begin(), data.end());
    auto minElement = *std::min_element(data.begin(), data.end());
    int count = std::count(data.begin(), data.end(), 500);
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    // Алгоритмы должны занять менее 100мс
    EXPECT_LT(duration.count(), 100);
    EXPECT_GE(maxElement, minElement);
    EXPECT_GE(count, 0);
}
