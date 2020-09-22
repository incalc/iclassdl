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

# Iclassdl
set(PROJECT_CORE_NAME ${PROJECT_NAME}_core)
add_library(${PROJECT_CORE_NAME} OBJECT)

set(PROJECT_EXE_NAME ${PROJECT_NAME}_cli)
add_executable(${PROJECT_EXE_NAME})

add_subdirectory(src)
target_include_directories(${PROJECT_CORE_NAME} PUBLIC include)

# Boost
find_package(Boost 1.69 REQUIRED COMPONENTS system thread)
target_include_directories(${PROJECT_CORE_NAME} PRIVATE ${Boost_INCLUDE_DIRS})
target_link_libraries(${PROJECT_CORE_NAME} PRIVATE ${Boost_LIBRARIES})

# FFmpeg
find_path(AVCODEC_INCLUDE_DIR libavcodec/avcodec.h)
find_library(AVCODEC_LIBRARY avcodec)

find_path(AVFORMAT_INCLUDE_DIR libavformat/avformat.h)
find_library(AVFORMAT_LIBRARY avformat)

find_path(AVUTIL_INCLUDE_DIR libavutil/avutil.h)
find_library(AVUTIL_LIBRARY avutil)

target_include_directories(${PROJECT_CORE_NAME} PRIVATE ${AVCODEC_INCLUDE_DIR} ${AVFORMAT_INCLUDE_DIR} ${AVUTIL_INCLUDE_DIR})
target_link_libraries(${PROJECT_CORE_NAME} PRIVATE ${AVCODEC_LIBRARY} ${AVFORMAT_LIBRARY} ${AVUTIL_LIBRARY})

# OpenSSL
find_package(OpenSSL REQUIRED)
target_link_libraries(${PROJECT_CORE_NAME} PRIVATE OpenSSL::SSL)

# Iclassdl Library
option(BUILD_STATIC_LIB "Build static library" OFF)
if (BUILD_STATIC_LIB)
    add_library(${PROJECT_NAME} STATIC $<TARGET_OBJECTS:${PROJECT_CORE_NAME}>)
else ()
    add_library(${PROJECT_NAME} SHARED $<TARGET_OBJECTS:${PROJECT_CORE_NAME}>)
endif ()
target_link_libraries(${PROJECT_NAME} PUBLIC ${PROJECT_CORE_NAME})

include(GNUInstallDirs)
install(TARGETS ${PROJECT_NAME}
        EXPORT ${PROJECT_NAME}
        INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

# Iclassdl Executable
target_link_libraries(${PROJECT_EXE_NAME} PRIVATE ${PROJECT_CORE_NAME})
