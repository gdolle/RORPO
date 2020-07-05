set(CPACK_PACKAGE_CONTACT "Odissey Merveille")
set(CPACK_PACKAGE_NAME "rorpo")
set(CPACK_PACKAGE_VENDOR "Path-Openings")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Ranking Orientation Responses of Path Openings (filters and segmentation)")
set(CPACK_PACKAGE_VERSION_MAJOR "1")
set(CPACK_PACKAGE_VERSION_MINOR "0")
set(CPACK_PACKAGE_VERSION_PATCH "0")
set(CPACK_PACKAGE_VERSION "${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}")
set(CPACK_PACKAGE_DIRECTORY "CPackPackages")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "Packages")
set(CPACK_CMAKE_GENERATOR "Unix Makefiles")
set(CPACK_GENERATOR "STGZ;TGZ;TZ;DEB")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/LICENSE")
set(CPACK_RESOURCE_FILE_README "${CMAKE_SOURCE_DIR}/README.adoc")
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
