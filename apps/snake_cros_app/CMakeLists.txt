# Кроссплатформенная игра «Змейка» на FastEngine (Desktop / в будущем iOS через общий код)
find_package(SDL2_ttf QUIET)
if(NOT SDL2_ttf_FOUND)
    find_package(PkgConfig QUIET)
    if(PkgConfig_FOUND)
        pkg_check_modules(SDL2_TTF QUIET SDL2_ttf)
    endif()
endif()

add_executable(snake_cros_app
    main.cpp
    Game.cpp
    GameUpdateSystem.cpp
)

target_include_directories(snake_cros_app PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}
)

target_link_libraries(snake_cros_app FastEngine)

if(SDL2_ttf_FOUND)
    target_link_libraries(snake_cros_app SDL2_ttf::SDL2_ttf)
    target_compile_definitions(snake_cros_app PRIVATE HAVE_SDL2_TTF=1)
elseif(SDL2_TTF_FOUND)
    target_include_directories(snake_cros_app PRIVATE ${SDL2_TTF_INCLUDE_DIRS})
    target_link_libraries(snake_cros_app ${SDL2_TTF_LIBRARIES})
    target_compile_options(snake_cros_app PRIVATE ${SDL2_TTF_CFLAGS_OTHER})
    target_compile_definitions(snake_cros_app PRIVATE HAVE_SDL2_TTF=1)
else()
    message(STATUS "SDL2_ttf not found: button labels disabled. Install with: brew install sdl2_ttf")
    target_compile_definitions(snake_cros_app PRIVATE HAVE_SDL2_TTF=0)
endif()

set_target_properties(snake_cros_app PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)
