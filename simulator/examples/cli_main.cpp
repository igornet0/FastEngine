#include "ProjectSimulator/CLI.h"
#include <iostream>
#include <exception>

int main(int argc, char* argv[]) {
    try {
        // Создаем CLI
        ProjectSimulator::CLI cli;
        
        // Инициализируем
        if (!cli.Initialize()) {
            std::cerr << "Failed to initialize CLI" << std::endl;
            return 1;
        }
        
        // Парсим аргументы
        auto args = cli.ParseArgs(argc, argv);
        
        // Выполняем команду
        int result = cli.ExecuteCommand(args);
        
        return result;
        
    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Unknown fatal error occurred" << std::endl;
        return 1;
    }
}

