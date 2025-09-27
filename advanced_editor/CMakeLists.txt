cmake_minimum_required(VERSION 3.16)
project(AdvancedGameEditor)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Поиск зависимостей
find_package(OpenGL REQUIRED)
find_package(SDL2 REQUIRED)
find_package(jsoncpp REQUIRED)

# Создаем исполняемый файл редактора
add_executable(AdvancedGameEditor advanced_editor.cpp)

# Линкуем с зависимостями
target_link_libraries(AdvancedGameEditor OpenGL::GL SDL2::SDL2 SDL2::SDL2main jsoncpp_lib)

# Настройки для macOS
if(APPLE)
    target_link_libraries(AdvancedGameEditor
        "-framework Cocoa"
        "-framework OpenGL"
    )
endif()

# Копируем ассеты
file(COPY ../assets DESTINATION ${CMAKE_BINARY_DIR})
