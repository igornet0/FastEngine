# Пример приложения
add_executable(ExampleApp
    main.cpp
    Game.cpp
    Game.h
)

target_link_libraries(ExampleApp FastEngine)

# Устанавливаем свойства
set_target_properties(ExampleApp PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)
