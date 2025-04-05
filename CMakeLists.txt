cmake_minimum_required(VERSION 3.16)

project(Ellipsoid VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Concurrent)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS OpenGL)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS OpenGLWidgets)

set(PROJECT_SOURCES
        main.cpp
        pmath.h
        scene.h
        helpers.h
        renderable.h
        renderable.cpp
        math/pvec4.h
        math/pmat4.h
        math/pquat.h
        scene/model.h
        scene/camera.h
        scene/projection.h
        window/add_object_dialog.h
        window/add_object_dialog.ui
        window/add_object_dialog.cpp
        window/main_window.h
        window/main_window.ui
        window/main_window.cpp
        window/open_gl_area.h
        window/open_gl_area.cpp
        common/position_params.h
        common/position_params.ui
        common/position_params.cpp
        common/single_color_phong_fragment_shader.glsl
        common/position_normal_vertex_shader.glsl
        cursor/screen_position_params.h
        cursor/screen_position_params.ui
        cursor/screen_position_params.cpp
        cursor/screen_position_dialog.h
        cursor/screen_position_dialog.ui
        cursor/screen_position_dialog.cpp
        cursor/cursor.h
        cursor/cursor.cpp
        point/point.h
        point/point.cpp
        torus/torus_params.h
        torus/torus_params.ui
        torus/torus_params.cpp
        torus/torus.h
        torus/torus.cpp
        torus/fragment_shader.glsl
        torus/vertex_shader.glsl
        ellipsoid/ellipsoid.h
        ellipsoid/ellipsoid.cpp
        ellipsoid/renderer.h
        ellipsoid/renderer.cpp
)

configure_file(common/single_color_phong_fragment_shader.glsl cursor/fragment_shader.glsl COPYONLY)
configure_file(common/position_normal_vertex_shader.glsl cursor/vertex_shader.glsl COPYONLY)
configure_file(common/single_color_phong_fragment_shader.glsl point/fragment_shader.glsl COPYONLY)
configure_file(common/position_normal_vertex_shader.glsl point/vertex_shader.glsl COPYONLY)
configure_file(torus/fragment_shader.glsl torus/fragment_shader.glsl COPYONLY)
configure_file(torus/vertex_shader.glsl torus/vertex_shader.glsl COPYONLY)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(Ellipsoid
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET Ellipsoid APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(Ellipsoid SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(Ellipsoid
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(Ellipsoid PRIVATE Qt${QT_VERSION_MAJOR}::Concurrent)
target_link_libraries(Ellipsoid PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
target_link_libraries(Ellipsoid PRIVATE Qt${QT_VERSION_MAJOR}::OpenGL)
target_link_libraries(Ellipsoid PRIVATE Qt${QT_VERSION_MAJOR}::OpenGLWidgets)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.Ellipsoid)
endif()
set_target_properties(Ellipsoid PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    # WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS Ellipsoid
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(Ellipsoid)
endif()
