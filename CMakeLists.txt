cmake_minimum_required(VERSION 3.16)

project(polymorphism_sequence LANGUAGES CXX)

include(CTest)

option(BUILD_GUI "Build the Dear ImGui visualizer" ON)

add_library(project_options INTERFACE)
target_compile_features(project_options INTERFACE cxx_std_17)
target_include_directories(project_options INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}")

if(MSVC)
    target_compile_options(project_options INTERFACE /W4 /permissive-)
else()
    target_compile_options(project_options INTERFACE -Wall -Wextra)
endif()

set(TEST_SOURCES
    tests/tests.cpp
    tests/matrix_tests.cpp
)

add_executable(main
    main.cpp
    ${TEST_SOURCES}
)
target_link_libraries(main PRIVATE project_options)

add_executable(tests
    ${TEST_SOURCES}
)
target_compile_definitions(tests PRIVATE TESTS_MAIN)
target_link_libraries(tests PRIVATE project_options)

if(BUILD_TESTING)
    add_test(NAME tests COMMAND tests)
endif()

if(BUILD_GUI)
    include(FetchContent)

    set(OpenGL_GL_PREFERENCE GLVND)
    find_package(OpenGL REQUIRED)

    set(GLFW_TARGET "")
    find_package(glfw3 CONFIG QUIET)
    if(TARGET glfw)
        set(GLFW_TARGET glfw)
    elseif(TARGET glfw::glfw)
        set(GLFW_TARGET glfw::glfw)
    elseif(TARGET glfw3)
        set(GLFW_TARGET glfw3)
    elseif(TARGET glfw3::glfw)
        set(GLFW_TARGET glfw3::glfw)
    endif()

    if(NOT GLFW_TARGET)
        find_package(PkgConfig QUIET)
        if(PkgConfig_FOUND)
            pkg_check_modules(GLFW3 QUIET IMPORTED_TARGET glfw3)
            if(TARGET PkgConfig::GLFW3)
                set(GLFW_TARGET PkgConfig::GLFW3)
            endif()
        endif()
    endif()

    if(NOT GLFW_TARGET)
        message(STATUS "System GLFW was not found; downloading GLFW with FetchContent")

        set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
        set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
        set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
        set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
        set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

        if(UNIX AND NOT APPLE)
            set(GLFW_BUILD_X11 ON CACHE BOOL "" FORCE)
            set(GLFW_BUILD_WAYLAND OFF CACHE BOOL "" FORCE)
        endif()

        FetchContent_Declare(
            glfw
            GIT_REPOSITORY https://github.com/glfw/glfw.git
            GIT_TAG 3.4
            GIT_SHALLOW TRUE
        )
        FetchContent_MakeAvailable(glfw)
        set(GLFW_TARGET glfw)
    endif()

    FetchContent_Declare(
        imgui
        GIT_REPOSITORY https://github.com/ocornut/imgui.git
        GIT_TAG v1.92.8
        GIT_SHALLOW TRUE
    )
    FetchContent_MakeAvailable(imgui)

    add_library(imgui STATIC
        "${imgui_SOURCE_DIR}/imgui.cpp"
        "${imgui_SOURCE_DIR}/imgui_draw.cpp"
        "${imgui_SOURCE_DIR}/imgui_tables.cpp"
        "${imgui_SOURCE_DIR}/imgui_widgets.cpp"
        "${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp"
        "${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp"
    )
    target_include_directories(imgui PUBLIC
        "${imgui_SOURCE_DIR}"
        "${imgui_SOURCE_DIR}/backends"
    )
    target_link_libraries(imgui PUBLIC
        ${GLFW_TARGET}
        OpenGL::GL
    )

    add_executable(gui
        gui/main_gui.cpp
    )
    target_link_libraries(gui PRIVATE
        project_options
        imgui
    )
endif()
