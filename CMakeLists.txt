cmake_minimum_required(VERSION 3.15)
project(iclassdl)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

if (MSVC)
    # warning level 4 and all warnings as errors
    add_compile_options(/W4 /WX)
else ()
    # lots of warnings and all warnings as errors
    #    add_compile_options(-Wall -Wextra -pedantic -Werror)
    add_compile_options(-Wall -Wextra -pedantic)
endif ()

include_directories(include)
add_subdirectory(src)

target_link_libraries(${PROJECT_NAME} PRIVATE avcodec avformat avutil)
