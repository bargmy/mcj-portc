cmake_minimum_required(VERSION 3.10)
project(RubyDung)

set(CMAKE_C_STANDARD 99)

set(SOURCES
    vec3.c aabb.c tesselator.c textures.c tile.c
    level.c frustum.c chunk.c level_renderer.c
    entity.c player.c character.c timer.c
    input.c font.c mobile_ui.c rubydung.c
)

if(ANDROID)
    add_library(rubydung-android SHARED ${SOURCES})
    target_compile_definitions(rubydung-android PRIVATE ANDROID_PORT)
    target_link_libraries(rubydung-android android log EGL GLESv1_CM z m)
    
    add_library(native_app_glue STATIC
        ${ANDROID_NDK}/sources/android/native_app_glue/android_native_app_glue.c
    )
    target_include_directories(rubydung-android PRIVATE
        ${ANDROID_NDK}/sources/android/native_app_glue
    )
    target_link_libraries(rubydung-android native_app_glue)
else()
    add_executable(rubydung ${SOURCES})
    
    if(WIN32)
        # For Windows, we might need to specify include/lib paths or use FetchContent
        find_package(OpenGL REQUIRED)
        # Assuming GLFW is provided or installed
        find_package(glfw3 QUIET)
        if(NOT glfw3_FOUND)
            message(STATUS "GLFW3 not found, assuming manual link for Windows builds")
            target_link_libraries(rubydung glfw3)
        else()
            target_link_libraries(rubydung glfw)
        endif()
        target_link_libraries(rubydung OpenGL::GL OpenGL::GLU zlib m)
    else()
        find_package(PkgConfig REQUIRED)
        pkg_check_modules(GLFW3 REQUIRED glfw3)
        target_link_libraries(rubydung ${GLFW3_LIBRARIES} GL GLU z m)
    endif()
endif()
