cmake_minimum_required(VERSION 3.21)

# Qt
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Gui)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Gui)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets OpenGLWidgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets OpenGLWidgets)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Network)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Network)

# Sources
file(GLOB_RECURSE APP_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
)

# Target
if (QT_VERSION_MAJOR EQUAL 6)
    qt_add_executable(Nyx
        MANUAL_FINALIZATION
        ${APP_SOURCES}
    )
else()
    add_executable(Nyx ${APP_SOURCES})
endif()

# Includes
target_include_directories(Nyx PRIVATE
    "${CMAKE_SOURCE_DIR}/Source"
    "${glew_SOURCE_DIR}"
)

# Compile defs that make life easier on Windows / GLM
target_compile_definitions(Nyx PRIVATE
    PROJECT_SOURCE_DIR="${CMAKE_SOURCE_DIR}"
    WIN32_LEAN_AND_MEAN
    NOMINMAX
    _CRT_SECURE_NO_WARNINGS
    GLM_ENABLE_EXPERIMENTAL
)

# Link
target_link_libraries(Nyx PRIVATE
    glfw glm::glm spdlog opengl32 Imgui stb
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::OpenGLWidgets
    Qt${QT_VERSION_MAJOR}::Gui
    Qt${QT_VERSION_MAJOR}::Network
)

if (QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(Nyx)
endif()

# 1) Copy all dependent runtime DLLs next to the exe (includes Qt6*.dll)
add_custom_command(TARGET Nyx POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E echo "Copying runtime DLLs..."
  COMMAND ${CMAKE_COMMAND} -E copy_if_different
          $<TARGET_RUNTIME_DLLS:Nyx> $<TARGET_FILE_DIR:Nyx>
  COMMAND_EXPAND_LISTS
)

# 2) Copy the required platform plugin via imported targets (robust across installs)
if (TARGET Qt6::QWindowsIntegrationPlugin)
  add_custom_command(TARGET Nyx POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E echo "Copying QPA plugin..."
    COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:Nyx>/plugins/platforms"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "$<TARGET_FILE:Qt6::QWindowsIntegrationPlugin>"
            "$<TARGET_FILE_DIR:Nyx>/plugins/platforms/"
  )
elseif (TARGET Qt5::QWindowsIntegrationPlugin)
  add_custom_command(TARGET Nyx POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E echo "Copying QPA plugin..."
    COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:Nyx>/plugins/platforms"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "$<TARGET_FILE:Qt5::QWindowsIntegrationPlugin>"
            "$<TARGET_FILE_DIR:Nyx>/plugins/platforms/"
  )
endif()