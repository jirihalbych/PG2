cmake_minimum_required(VERSION 3.27)
project(pg_testik)
add_executable(pg_testik app.cpp)
set(CMAKE_CXX_STANDARD 17)

find_library(IrrKlang_LIBRARIES irrKlang
        ${CMAKE_CURRENT_SOURCE_DIR}/bin/linux-gcc
)

find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)
find_package(PNG REQUIRED)
add_library(irrKlang SHARED IMPORTED)
set_target_properties(irrKlang PROPERTIES LINKER_LANGUAGE CXX
        IMPORTED_LOCATION "${CMAKE_CURRENT_LIST_DIR}/bin/linux-gcc/libIrrKlang.so"
        INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_LIST_DIR}/include")
set_target_properties(irrKlang PROPERTIES LINKER_LANGUAGE CXX)
set_target_properties(irrKlang PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

include_directories(${PNG_INCLUDE_DIR})
include_directories( ${OpenCV_INCLUDE_DIRS} )
include_directories(${OPENGL_INCLUDE_DIR})
include_directories(${GLEW_INCLUDE_DIRS})

target_include_directories(
        ${PROJECT_NAME}
        PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/include
)


target_link_libraries(pg_testik ${PNG_LIBRARY} ${OPENGL_LIBRARIES} ${GLEW_LIBRARIES} glfw irrKlang)