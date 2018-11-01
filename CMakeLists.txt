cmake_minimum_required(VERSION 3.1..3.12)

if(${CMAKE_VERSION} VERSION_LESS 3.12)

    cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})

endif()

project(Labyrinth VERSION 0.1.0
                  DESCRIPTION "A Labyrinth project"
                  LANGUAGES C)

include_directories(
    ${PROJECT_SOURCE_DIR}/src
)

file(GLOB_RECURSE all_c_src
    "${PROJECT_SOURCE_DIR}/src/*.c"
)

add_library(LabLexer STATIC ${all_c_src})
set_property(TARGET LabLexer PROPERTY C_STANDARD 11)

if(CMAKE_SIZEOF_VOID_P EQUAL 8)     # 64 bit

    target_compile_definitions(LabLexer PUBLIC LAB_64_BIT)

elseif(CMAKE_SIZEOF_VOID_P EQUAL 4) # 32 bit

    target_compile_definitions(LabLexer PUBLIC LAB_32_BIT)

endif()