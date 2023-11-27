cmake_minimum_required(VERSION 3.0.0)
project(TinyMPC VERSION 0.2.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include/Eigen)

# Options to control the build process
option(BUILD_TESTING "Build the test suite" OFF)
option(BUILD_EXAMPLE "Build example applications" OFF)
option(BUILD_CODEGEN "Build codegen components" OFF)

# Add src/tinympc directory
add_subdirectory(src/tinympc)

# Conditionally add examples and tests based on the specified options
if(BUILD_EXAMPLE)
  add_subdirectory(examples)
endif()

if(BUILD_TESTING)
  add_subdirectory(test)
endif()

install(TARGETS tinympc 
        ARCHIVE DESTINATION lib
        INCLUDES DESTINATION include
    )

# Install the header files
file(GLOB HEADERS "src/tinympc/*.hpp")
install(FILES ${HEADERS} DESTINATION include/tinympc)
file(GLOB EIGEN_HEADER "include/Eigen/Eigen.h")
install(FILES ${EIGEN_HEADER} DESTINATION include/)


