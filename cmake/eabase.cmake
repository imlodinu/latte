CPMAddPackage(
    NAME EABase
    GITHUB_REPOSITORY koi-so/EABase
    GIT_TAG master
    VERSION 2.09.06
)

# if(EABase_ADDED)
#   add_library(EABase INTERFACE)
#   target_include_directories(EABase INTERFACE ${EABase_SOURCE_DIR}/include/Common)
# endif()