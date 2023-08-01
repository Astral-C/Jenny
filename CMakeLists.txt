cmake_minimum_required(VERSION 3.8)
project(jenny)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_BUILD_TYPE Debug)

add_subdirectory(lib)

file(GLOB_RECURSE JENNY_SRC
    # jenny
    "src/*.cpp"
    "src/*.c"
    "include/*.hpp"
    "include/*.h"
    
    # ImGuiFileDialog
    "lib/ImGuiFileDialog/*.cpp"
)

find_package(Iconv REQUIRED)

add_executable(jenny ${JENNY_SRC})
target_include_directories(jenny PUBLIC include include/util lib/glfw/include lib/ImGuiFileDialog lib/libgctools/include lib/fmt/include ${Iconv_INCLUDE_DIRS})

if(UNIX)
target_link_libraries(jenny PUBLIC imgui glfw gctools fmt j3dultra ${ICONV_LIBRARIES})
endif(UNIX)

if(WIN32)
target_link_libraries(jenny PUBLIC imgui glfw gctools fmt j3dultra ${ICONV_LIBRARIES} ssp)
endif(WIN32)