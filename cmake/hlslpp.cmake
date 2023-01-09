CPMAddPackage(
    NAME hlslpp
    GITHUB_REPOSITORY redorav/hlslpp
    GIT_TAG 3.2.1
    VERSION 3.2.1
)

add_library(hlslpp INTERFACE)
target_include_directories(hlslpp INTERFACE ${hlslpp_SOURCE_DIR}/include)
target_compile_definitions(hlslpp INTERFACE
    hlslpp_FEATURE_TRANSFORM # Enable transformation matrices
    hlslpp_LOGICAL_LAYOUT=0  # Set row-major logical layout
    hlslpp_COORDINATES=0     # Set left-handed coordinate system
)

if(MSVC)
    target_sources(hlslpp INTERFACE ${hlslpp_SOURCE_DIR}/include/hlsl++.natvis)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU") # GCC
    target_compile_options(hlslpp INTERFACE -Wno-deprecated-copy)
endif()