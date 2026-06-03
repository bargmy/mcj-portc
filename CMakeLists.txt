cmake_minimum_required(VERSION 3.10.2)
project(RubyDungAndroid)

# Add Android support
if(ANDROID)
    add_library(rubydung-android SHARED
        vec3.c aabb.c tesselator.c textures.c tile.c
        level.c frustum.c chunk.c level_renderer.c
        entity.c player.c character.c timer.c
        input.c font.c mobile_ui.c rubydung.c
    )
    
    # Android specific flags and libraries
    target_compile_definitions(rubydung-android PRIVATE ANDROID_PORT)
    target_link_libraries(rubydung-android
        android
        log
        EGL
        GLESv1_CM
        z
        m
    )
    
    # Link native_app_glue
    set(CMAKE_STATIC_LIBRARY_PREFIX "")
    add_library(native_app_glue STATIC
        ${ANDROID_NDK}/sources/android/native_app_glue/android_native_app_glue.c
    )
    target_include_directories(rubydung-android PRIVATE
        ${ANDROID_NDK}/sources/android/native_app_glue
    )
    target_link_libraries(rubydung-android native_app_glue)
else()
    # Desktop build
    add_executable(rubydung
        vec3.c aabb.c tesselator.c textures.c tile.c
        level.c frustum.c chunk.c level_renderer.c
        entity.c player.c character.c timer.c
        input.c font.c mobile_ui.c rubydung.c
    )
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(GLFW3 REQUIRED glfw3)
    target_link_libraries(rubydung ${GLFW3_LIBRARIES} GL GLU z m)
endif()
