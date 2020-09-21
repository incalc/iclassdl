cmake_minimum_required(VERSION 3.15)
project(iclassdl
        VERSION 0.1.0
        DESCRIPTION "Inha Univ. I-Class Course Video Downloader"
        HOMEPAGE_URL https://github.com/incalc/iclassdl
        LANGUAGES C CXX)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

if (MSVC)
    # warning level 4 and all warnings as errors
    #    add_compile_options(/W4 /WX)
    add_compile_options(/W4)
else ()
    # lots of warnings and all warnings as errors
    #    add_compile_options(-Wall -Wextra -pedantic -Werror)
    add_compile_options(-Wall -Wextra -pedantic)
endif ()

include_directories(include)
add_subdirectory(src)

# Boost
find_package(Boost 1.69 REQUIRED COMPONENTS system thread)
include_directories(${Boost_INCLUDE_DIRS})
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
