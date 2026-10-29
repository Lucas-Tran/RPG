cmake_minimum_required(VERSION 3.5.0)
project(RPG VERSION 0.10 LANGUAGES C CXX)

set(CMAKE_C_STANDARD 17)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)



include_directories(${CMAKE_SOURCE_DIR}/Dependencies/Headers ${CMAKE_SOURCE_DIR}/Headers)
link_directories(${CMAKE_SOURCE_DIR}/Dependencies/Libraries)

file(GLOB_RECURSE SOURCE_FILES CONFIGURE_DEPENDS
        ${CMAKE_SOURCE_DIR}/Source/*.cpp ${CMAKE_SOURCE_DIR}/Source/*.c
        ${CMAKE_SOURCE_DIR}/Dependencies/Source/*.cpp ${CMAKE_SOURCE_DIR}/Dependencies/Source/*.c
)

add_executable(${CMAKE_PROJECT_NAME} ${SOURCE_FILES})

target_compile_definitions(RPG PRIVATE ENABLE_LEVEL_EDITOR)

target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE raylib)

if(WIN32)
    target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE winmm gdi32)
elseif(APPLE)
    target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE
            "-framework OpenGL"
            "-framework Cocoa"
            "-framework IOKit"
            "-framework CoreVideo"
            "-framework QuartzCore"
            "-framework Metal"
    )
else()
    target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE pthread m X11)
endif()

file(COPY ${CMAKE_SOURCE_DIR}/Resources/ DESTINATION ${CMAKE_BINARY_DIR})