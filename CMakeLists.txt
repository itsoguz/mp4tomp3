cmake_minimum_required(VERSION 3.10)

# Explicitly tell FindOpenGL to prefer GLVND libraries.
set(OpenGL_GL_PREFERENCE "GLVND")

project(mp4tomp3 VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
#set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_AUTOMOC ON)

# Specify where to place the executable binaries
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Find Qt6 packages
find_package(Qt6 COMPONENTS Core Widgets REQUIRED)

# Add the source subdirectory
#add_subdirectory(src)

set(PROJECT_SOURCES
        ${SOURCE_DIR}/main.cpp
    )

#set(PROJECT_HEADERS
#        ${INCLUDE_DIR}/AppManager.hpp
#    )

#add_executable(${PROJECT_NAME} ${PROJECT_SOURCES} ${PROJECT_HEADERS})
add_executable(${PROJECT_NAME} ${PROJECT_SOURCES})

# Link against Qt6 libraries
target_link_libraries(${PROJECT_NAME}
    PRIVATE
    Qt6::Core
    Qt6::Widgets
)

# Include directories for Qt and project headers
target_include_directories(${PROJECT_NAME} PRIVATE ${INCLUDE_DIR})

# Copy audio to build dir
#add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
#    COMMAND ${CMAKE_COMMAND} -E copy_directory
#    ${CMAKE_SOURCE_DIR}/audio $<TARGET_FILE_DIR:${PROJECT_NAME}>/audio
#)
