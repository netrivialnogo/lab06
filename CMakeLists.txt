
cmake_minimum_required(VERSION 3.10...3.28)


set(user.email "kbelovwowcirkle@gmail.com")
set(user.name "netrivialnogo")

# Compiler settings
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_COMPILER "/usr/bin/g++")


set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/Debug")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/Release")


option(COVERAGE "Enable code coverage" OFF)
option(BUILD_TESTS "Build tests" ON)


project(TestRunning
    VERSION 0.1.0
    DESCRIPTION "Banking application test framework"
    LANGUAGES CXX
)


if(NOT DEFINED GITHUB_EMAIL)
    set(GITHUB_EMAIL "kbelovwowcirkle@gmail.com")  
endif()

if(NOT DEFINED GITHUB_USERNAME)
    set(GITHUB_USERNAME "netrivialnogo")       
endif()


if(BUILD_TESTS)
    if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/googletest/CMakeLists.txt")
        message(FATAL_ERROR 
            "GoogleTest submodule not found!\n"
            "Run: git submodule update --init --recursive"
        )
    endif()
    
    add_subdirectory(googletest)
endif()


add_subdirectory(banking)


if(BUILD_TESTS)
    add_executable(RunTest test.cpp)
    
    if(COVERAGE)
        target_compile_options(RunTest PRIVATE --coverage)
        target_link_libraries(RunTest PRIVATE --coverage)
    endif()

    target_include_directories(RunTest PRIVATE banking)
    target_link_libraries(RunTest PRIVATE 
        gtest 
        gtest_main 
        gmock
        banking
    )
endif()


install(TARGETS RunTest
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    COMPONENT tests
)

install(DIRECTORY banking/
    DESTINATION include
    FILES_MATCHING 
    PATTERN "*.h"
    PATTERN "*.hpp"
)

include(InstallRequiredSystemLibraries)

set(CPACK_PACKAGE_VENDOR "${GITHUB_USERNAME}")
set(CPACK_PACKAGE_CONTACT "${GITHUB_EMAIL}")
set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
set(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/DESCRIPTION")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Banking application test framework")
set(CPACK_RESOURCE_FILE_README "${CMAKE_CURRENT_SOURCE_DIR}/README.md")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")


set(CPACK_RPM_PACKAGE_NAME "testrunning-devel")
set(CPACK_RPM_PACKAGE_GROUP "Development/Tools")
set(CPACK_RPM_CHANGELOG_FILE "${CMAKE_CURRENT_SOURCE_DIR}/ChangeLog.md")
set(CPACK_RPM_PACKAGE_RELEASE 1)


set(CPACK_DEBIAN_PACKAGE_NAME "libtestrunning-dev")
set(CPACK_DEBIAN_PACKAGE_PREDEPENDS "cmake >= 3.10")
set(CPACK_DEBIAN_PACKAGE_RELEASE 1)


if(WIN32)
    set(CPACK_GENERATOR "WIX")
    set(CPACK_WIX_UPGRADE_GUID "D9E8F7A6-B5C4-4D3E-9F8A-1B2C3D4E5F60")  # New GUID
    set(CPACK_WIX_ARCHITECTURE "x64")
    
    if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE.rtf")
        file(WRITE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE.rtf" " ")
    endif()
    set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE.rtf")
else()
    set(CPACK_GENERATOR "TGZ;DEB;RPM")
    set(CPACK_DEBIAN_PACKAGE_MAINTAINER 
        "netrivialnogo <kbelovwowcirkle@gmail.com>"
    )
    set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE "amd64")
endif()

include(CPack)
