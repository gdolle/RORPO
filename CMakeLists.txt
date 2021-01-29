cmake_minimum_required(VERSION 3.2)
cmake_policy(SET CMP0048 NEW)

project(RORPO
        VERSION 1.0.0
        DESCRIPTION "Ranking Orientation Responses of Path Openings"
        LANGUAGES C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(UNIX)
	SET(LIB_TYPE SHARED)
else()
	SET(LIB_TYPE STATIC)
endif()

# Build Type
if(NOT CMAKE_BUILD_TYPE)
   SET(CMAKE_BUILD_TYPE Release
       CACHE STRING "Choose the type of build : Debug or Release."
       FORCE)
endif()
message("Build Type: " ${CMAKE_BUILD_TYPE})

include_directories(
            libRORPO/include
            Image/include
            pyRORPO)

if (NOT ${CMAKE_BUILD_TYPE} STREQUAL "Release")
	enable_testing()
endif()
add_subdirectory(libRORPO)
add_subdirectory(RORPO_multiscale_Usage)
add_subdirectory(pyRORPO)

########################################
# CPACK - Packages
########################################

# Generate packages
include(InstallRequiredSystemLibraries)

set(CPACK_PACKAGE_CONTACT "Odissey Merveille")
set(CPACK_PACKAGE_NAME ${PROJECT_NAME})
set(CPACK_PACKAGE_VENDOR "Path-Openings")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY ${PROJECT_DESCRIPTION})
set(CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERSION_PATCH})
set(CPACK_PACKAGE_VERSION "${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}")
set(CPACK_PACKAGE_DIRECTORY "pkg")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "Packages")
set(CPACK_CMAKE_GENERATOR "Unix Makefiles")
set(CPACK_GENERATOR "STGZ;TGZ;TZ;DEB")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/LICENSE.md")
set(CPACK_RESOURCE_FILE_README "${CMAKE_SOURCE_DIR}/README.md")
set(CPACK_SOURCE_GENERATOR "TGZ;TZ")
#set(CPACK_SOURCE_IGNORE_FILES "/CVS/;/\\.svn/;\\.swp$;\\.#;/#;.*~;cscope.*;/\\.git.*")
#set(CPACK_SYSTEM_NAME "linux-x86_64")
#set(CPACK_TOPLEVEL_TAG "linux-x86_64")
if(WIN32 AND NOT UNIX)
    set(CPACK_NSIS_DISPLAY_NAME "${CPACK_PACKAGE_INSTALL_DIRECTORY} rorpo")
    set(CPACK_NSIS_HELP_LINK "http:\\\\\\\\path-openings.github.io")
    set(CPACK_NSIS_URL_INFO_ABOUT "http:\\\\\\\\path-openings.github.io")
else()
    set(CPACK_STRIP_FILES "bin/RORPO_multiscale_usage")
    set(CPACK_SOURCE_STRIP_FILES "")
endif()
#set(CPACK_PACKAGE_EXECUTABLES "MyExecutable" "My Executable")

# DEBIAN 
#set(CPACK_DEBIAN_PACKAGE_EPOCH "1")
set(CPACK_DEBIAN_PACKAGE_DEPENDS "\
libdocopt-dev \
libfftw3-dev \
libinsighttoolkit4-dev \
libgdcm-dev \
libgdcm-tools \
libvtkgdcm-dev \
libvtkgdcm-tools \
")

# Module.
include(CPack)
