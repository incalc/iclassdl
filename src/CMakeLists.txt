option(BUILD_SHARED_LIBS "Build shared library" ON)
add_library(${PROJECT_NAME}
        media_impl.c
        media.cpp
        session.cpp)

add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

add_executable(${PROJECT_NAME}_cli main.cpp)
