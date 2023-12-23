# ##############################################################################
# CMAKE VERSION
cmake_minimum_required(VERSION 3.5)

# PROJECT VERSION
project(pacnanogui VERSION 0.1 LANGUAGES CXX)

# CMAKE CONFIGURATION
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# CPLUS CONFIGURATION
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ##############################################################################
# DEFINE PACKAGES
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)
find_package(Qt6 REQUIRED COMPONENTS Sql)

# ##############################################################################
# VTK CONFIGURATION
# Set the cmake folder fo VTK
# WINDOWS PLATFORM
if(WIN32)
    message("-- The program is complied in WINDOWS system ...")
    if (CMAKE_BUILD_TYPE STREQUAL "Debug")
        message("-- Setting enronment variables for Debug mode ...")
        set(ENV{VTK_DIR} "D:/VTK/Debug/lib/cmake/vtk-9.3")
    elseif (CMAKE_BUILD_TYPE STREQUAL "Release")
        message("-- Setting enronment variables for Release mode ...")
        set(ENV{VTK_DIR} "D:/VTK/Release/lib/cmake/vtk-9.3")
    endif()
endif()
# UNIX/LINUX PLATFORM
if (UNIX)
    message("-- The program is complied in LINUX system ...")
    set(ENV{VTK_DIR} "/opt/vtk/9.3.0/lib64/cmake/vtk-9.3")
    if (CMAKE_BUILD_TYPE STREQUAL "Debug")
        message("-- Setting enronment variables for Debug mode ...")
    elseif (CMAKE_BUILD_TYPE STREQUAL "Release")
        message("-- Setting enronment variables for Release mode ...")
    endif()
endif()
# MACOS/APPLE PLATFORM
if (APPLE)
    message("-- The program is complied in MACOS/APPLE system ...")
    set(ENV{VTK_DIR} "/Volumes/opt/Softwares/vtk/9.3.0/lib/cmake/vtk-9.3")
    if (CMAKE_BUILD_TYPE STREQUAL "Debug")
        message("-- Setting enronment variables for Debug mode ...")
    elseif (CMAKE_BUILD_TYPE STREQUAL "Release")
        message("-- Setting enronment variables for Release mode ...")
    endif()
endif()
find_package(VTK 9.3 REQUIRED COMPONENTS)

# ##############################################################################
# DFINE THE MAIN FUNCTION
set(PROJECT_SOURCES
        main.cpp
        pacnano.cpp
        pacnano.h
        pacnano.ui
)
# OTHER EXECUTABLE SOURCE FILES
if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(pacnanogui
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        icons.qrc
        manager.ui
        manager.cpp manager.h
        rename.cpp rename.h rename.ui
        remote.cpp remote.h remote.ui
        open.cpp open.h open.ui
        model.cpp model.h model.ui
        set.cpp set.h set.ui
        messagebox.cpp messagebox.h messagebox.ui
        material.cpp material.h material.ui
        project.cpp project.h project.ui
        viewer.h viewer.cpp
        optimization.h optimization.cpp optimization.ui
        field.h field.cpp
        matassign.h matassign.cpp matassign.ui
        camera.h camera.cpp camera.ui
        pick.h pick.cpp
        post.h post.cpp post.ui
    )

# ##############################################################################
# Define target properties for Android with Qt 6 as:
# set_property(TARGET pacnanogui APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#            ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html
# target-creation
else()
    if(ANDROID)
        add_library(pacnanogui SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(pacnanogui
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(pacnanogui PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt6::Sql)
target_link_libraries(pacnanogui PRIVATE ${VTK_LIBRARIES})

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.pacnanogui)
endif()
set_target_properties(pacnanogui PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING
    ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS pacnanogui
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(pacnanogui)
endif()
