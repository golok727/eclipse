cmake_minimum_required(VERSION 3.21)
project(eclipse_tetris VERSION 1.0.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(MSVC)
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
    add_compile_options(/utf-8)
endif()

find_package(OpenGL REQUIRED)
find_package(SDL2 REQUIRED)
find_package(glm REQUIRED)
find_package(Python3 REQUIRED COMPONENTS Interpreter)

set(ECLIPSE_SOURCES
    core/audioclip.cpp
    core/clock.cpp
    core/imguiwindow.cpp
    core/window.cpp
    graphics/framebuffer.cpp
    graphics/helpers.cpp
    graphics/mesh.cpp
    graphics/rendercommands.cpp
    graphics/shader.cpp
    graphics/texture.cpp
    input/keyboard.cpp
    input/mouse.cpp
    managers/assetmanager.cpp
    managers/audiomanager.cpp
    managers/layeredtilemaploader.cpp
    managers/logmanager.cpp
    managers/rendermanager.cpp
    managers/scenemanager.cpp
    managers/tiledmaploader.cpp
    src/engine.cpp
    systems/animation_system.cpp
    systems/camera_system.cpp
    systems/collision_system.cpp
    systems/movement_system.cpp
    systems/npc_system.cpp
    systems/pathfinding.cpp
    systems/render_system.cpp
    external/glad/src/glad.c
    eclipse/include/external/imgui/imgui.cpp
    eclipse/include/external/imgui/imgui_draw.cpp
    eclipse/include/external/imgui/imgui_impl_opengl3.cpp
    eclipse/include/external/imgui/imgui_impl_sdl2.cpp
    eclipse/include/external/imgui/imgui_stdlib.cpp
    eclipse/include/external/imgui/imgui_tables.cpp
    eclipse/include/external/imgui/imgui_widgets.cpp
)

add_library(eclipse STATIC ${ECLIPSE_SOURCES})
target_include_directories(eclipse
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/eclipse/include
        ${CMAKE_CURRENT_SOURCE_DIR}/external/glad/include
        ${CMAKE_CURRENT_SOURCE_DIR}/external/spdlog/include
)
target_link_libraries(eclipse PUBLIC OpenGL::GL)
if(TARGET glm::glm-header-only)
    target_link_libraries(eclipse PUBLIC glm::glm-header-only)
else()
    target_link_libraries(eclipse PUBLIC glm::glm)
endif()

if(TARGET SDL2::SDL2)
    target_link_libraries(eclipse PUBLIC SDL2::SDL2)
elseif(TARGET SDL2::SDL2-static)
    target_link_libraries(eclipse PUBLIC SDL2::SDL2-static)
else()
    target_include_directories(eclipse PUBLIC ${SDL2_INCLUDE_DIRS})
    target_link_libraries(eclipse PUBLIC ${SDL2_LIBRARIES})
endif()

if(WIN32)
    target_compile_definitions(eclipse PUBLIC ECLIPSE_PLATFORM_WINDOWS)
elseif(APPLE)
    target_compile_definitions(eclipse PUBLIC ECLIPSE_PLATFORM_MAC)
else()
    target_compile_definitions(eclipse PUBLIC ECLIPSE_PLATFORM_LINUX)
endif()

if(MSVC)
    target_compile_options(eclipse PRIVATE /WX)
else()
    target_compile_options(eclipse PRIVATE -Werror)
endif()

target_compile_definitions(eclipse PUBLIC
    $<$<CONFIG:Debug>:ECLIPSE_CONFIG_DEBUG;DEBUG>
    $<$<NOT:$<CONFIG:Debug>>:ECLIPSE_CONFIG_RELEASE;NDEBUG>
)

add_executable(tetrisgame
    games/tetris/main.cpp
    games/tetris/tetris_app.cpp
)
target_link_libraries(tetrisgame PRIVATE eclipse)
if(TARGET SDL2::SDL2main)
    target_link_libraries(tetrisgame PRIVATE SDL2::SDL2main)
endif()

if(WIN32)
    add_custom_command(TARGET tetrisgame POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
                $<TARGET_RUNTIME_DLLS:tetrisgame>
                $<TARGET_FILE_DIR:tetrisgame>
        COMMAND_EXPAND_LISTS
        COMMENT "Copying runtime DLLs"
    )
endif()

set_target_properties(tetrisgame PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/tetrisgame"
    RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/tetrisgame"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/tetrisgame"
    RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO "${CMAKE_BINARY_DIR}/tetrisgame"
)

add_custom_command(TARGET tetrisgame POST_BUILD
    COMMAND ${Python3_EXECUTABLE}
            ${CMAKE_CURRENT_SOURCE_DIR}/tools/package_assets.py
            ${CMAKE_CURRENT_SOURCE_DIR}/assets
            $<TARGET_FILE_DIR:tetrisgame>/assets
            tetris
    COMMENT "Packaging Tetris assets"
    VERBATIM
)

include(CTest)
if(BUILD_TESTING)
    add_executable(eclipsetests tests/assetmanager_tests.cpp)
    target_link_libraries(eclipsetests PRIVATE eclipse)
    add_test(NAME assetmanager COMMAND eclipsetests)
    set_tests_properties(assetmanager PROPERTIES
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    )
endif()

install(TARGETS tetrisgame RUNTIME DESTINATION . BUNDLE DESTINATION .)
if(WIN32)
    install(FILES $<TARGET_RUNTIME_DLLS:tetrisgame> DESTINATION .)
endif()
install(DIRECTORY "${CMAKE_BINARY_DIR}/tetrisgame/assets" DESTINATION .)
