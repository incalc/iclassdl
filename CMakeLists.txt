cmake_minimum_required(VERSION 3.15)
project(iclassdl
        VERSION 0.1.0
        DESCRIPTION "Inha Univ. I-Class Course Video Downloader"
        HOMEPAGE_URL https://github.com/incalc/iclassdl
        LANGUAGES C CXX)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

if (MSVC)
    add_compile_options(/W4)
else ()
    add_compile_options(-Wall -Wextra -pedantic)
endif ()

add_subdirectory(src)

# Boost
find_package(Boost 1.69 REQUIRED COMPONENTS system thread)
target_include_directories(${PROJECT_NAME} PRIVATE ${Boost_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} PRIVATE ${Boost_LIBRARIES})

# FFmpeg
find_path(AVCODEC_INCLUDE_DIR libavcodec/avcodec.h)
find_library(AVCODEC_LIBRARY avcodec)

find_path(AVFORMAT_INCLUDE_DIR libavformat/avformat.h)
find_library(AVFORMAT_LIBRARY avformat)

find_path(AVUTIL_INCLUDE_DIR libavutil/avutil.h)
find_library(AVUTIL_LIBRARY avutil)

target_include_directories(${PROJECT_NAME} PRIVATE ${AVCODEC_INCLUDE_DIR} ${AVFORMAT_INCLUDE_DIR} ${AVUTIL_INCLUDE_DIR})
target_link_libraries(${PROJECT_NAME} PRIVATE ${AVCODEC_LIBRARY} ${AVFORMAT_LIBRARY} ${AVUTIL_LIBRARY})

# OpenSSL
find_package(OpenSSL REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE OpenSSL::SSL)

# Iclassdl Library
include(GNUInstallDirs)

set(TARGET_EXPORT_NAME ${PROJECT_NAME}-targets)
set(INSTALL_CONFIG_DIR ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME})
set(INSTALL_CONFIG_FILE ${PROJECT_NAME}-config.cmake)
set(INSTALL_CONFIG_VERSION_FILE ${PROJECT_NAME}-config-version.cmake)

target_include_directories(${PROJECT_NAME}
        PUBLIC $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/include> $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>)

include(GenerateExportHeader)
generate_export_header(${PROJECT_NAME}
        EXPORT_FILE_NAME ${CMAKE_BINARY_DIR}/include/${PROJECT_NAME}/export.h)

include(CMakePackageConfigHelpers)
configure_package_config_file(${CMAKE_CURRENT_LIST_DIR}/cmake/${INSTALL_CONFIG_FILE}.in ${CMAKE_CURRENT_BINARY_DIR}/cmake/${INSTALL_CONFIG_FILE}
        INSTALL_DESTINATION ${INSTALL_CONFIG_DIR})
write_basic_package_version_file(${CMAKE_CURRENT_BINARY_DIR}/cmake/${INSTALL_CONFIG_VERSION_FILE}
        COMPATIBILITY SameMajorVersion)

install(DIRECTORY ${CMAKE_BINARY_DIR}/include/${PROJECT_NAME}
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(DIRECTORY ${CMAKE_SOURCE_DIR}/include/${PROJECT_NAME}
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

install(TARGETS ${PROJECT_NAME}
        EXPORT ${TARGET_EXPORT_NAME}
        INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
install(EXPORT ${TARGET_EXPORT_NAME}
        DESTINATION ${INSTALL_CONFIG_DIR}
        NAMESPACE ${PROJECT_NAME}::
        FILE ${TARGET_EXPORT_NAME}.cmake)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/cmake/${INSTALL_CONFIG_FILE} ${CMAKE_CURRENT_BINARY_DIR}/cmake/${INSTALL_CONFIG_VERSION_FILE}
        DESTINATION ${INSTALL_CONFIG_DIR})

export(EXPORT ${TARGET_EXPORT_NAME}
        NAMESPACE ${PROJECT_NAME}::
        FILE ${CMAKE_BINARY_DIR}/cmake/${TARGET_EXPORT_NAME}.cmake)

# Iclassdl Executable
target_link_libraries(${PROJECT_NAME}_cli PRIVATE ${PROJECT_NAME}::${PROJECT_NAME})
