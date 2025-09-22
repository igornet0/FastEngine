# Примеры для Project Simulator

# Основное приложение симулятора
add_executable(SimulatorApp main.cpp)

target_link_libraries(SimulatorApp ProjectSimulator)

# Устанавливаем директории для ассетов
if(IOS)
    set_target_properties(SimulatorApp PROPERTIES
        MACOSX_BUNDLE TRUE
        MACOSX_BUNDLE_BUNDLE_NAME "Project Simulator"
        MACOSX_BUNDLE_BUNDLE_VERSION "1.0.0"
        MACOSX_BUNDLE_SHORT_VERSION_STRING "1.0"
    )
endif()

# Копируем примеры проектов в выходную директорию
file(COPY ../projects/sample_projects DESTINATION ${CMAKE_BINARY_DIR}/projects)

