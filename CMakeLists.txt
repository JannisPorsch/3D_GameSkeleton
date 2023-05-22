cmake_minimum_required(VERSION 3.13.4)


project(3D_GameSkeleton)


find_package(OpenGL REQUIRED)


FILE(GLOB MAIN
    src/main.c
    src/main.cpp
    src/main.h
    src/main.hpp
)

add_executable(${PROJECT_NAME} ${MAIN})


target_include_directories(${PROJECT_NAME}
    PUBLIC src
    PUBLIC dependencies/GLFW/include
    PUBLIC dependencies/glad/include
    PUBLIC dependencies/CGLM/include
    PUBLIC dependencies/stb_image
    PUBLIC ${OPENGL_INCLUDE_DIRS}
)


FILE(GLOB SUBDIRS ${CMAKE_CURRENT_SOURCE_DIR}/src/*)
FOREACH(dir ${SUBDIRS})
    IF(IS_DIRECTORY ${dir})
        
        FILE(GLOB_RECURSE sources 
            ${dir}/*.c
            ${dir}/*.cpp
        )

        IF(sources)
            get_filename_component(tmp ${dir} NAME)
            string(REPLACE " " "_" tmp ${tmp})

            add_library(${tmp} STATIC ${sources})
            target_include_directories(${tmp}
                PUBLIC src
                PUBLIC dependencies/GLFW/include
                PUBLIC dependencies/glad/include
                PUBLIC dependencies/CGLM/include
                PUBLIC dependencies/stb_image
                PUBLIC ${OPENGL_INCLUDE_DIRS}
            )
        ENDIF()
    ENDIF()
ENDFOREACH()


add_subdirectory(dependencies)


set(LIBRARIES "engine;render;io")

target_link_libraries(${PROJECT_NAME} ${LIBRARIES} ${OPENGL_LIBRARIES} glad glfw cglm stb_image)
