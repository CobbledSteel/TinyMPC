# Create a list of source files
set(TINY_MPC_SOURCES
    admm.cpp
)

# Conditionally include codegen.cpp if the BUILD_CODEGEN option is ON
if(BUILD_CODEGEN)
    list(APPEND TINY_MPC_SOURCES codegen.cpp)
endif()

# Add library
add_library(tinympc STATIC ${TINY_MPC_SOURCES})

# Include directories
target_include_directories(tinympc PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..) # Include src/ directory

