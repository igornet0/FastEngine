# Поиск библиотеки GLM (OpenGL Mathematics)

find_path(GLM_INCLUDE_DIR 
    NAMES glm/glm.hpp
    PATHS
        ${GLM_ROOT_DIR}
        ${GLM_ROOT_DIR}/include
        /usr/include
        /usr/local/include
        /opt/local/include
        /opt/homebrew/include
        ${CMAKE_SOURCE_DIR}/thirdparty/glm
        ${CMAKE_CURRENT_SOURCE_DIR}/../thirdparty/glm
)

if(GLM_INCLUDE_DIR)
    set(GLM_FOUND TRUE)
    set(GLM_INCLUDE_DIRS ${GLM_INCLUDE_DIR})
    message(STATUS "Found GLM: ${GLM_INCLUDE_DIR}")
else()
    set(GLM_FOUND FALSE)
    message(WARNING "GLM not found. Consider installing it or adding to thirdparty/")
endif()

mark_as_advanced(GLM_INCLUDE_DIR)
