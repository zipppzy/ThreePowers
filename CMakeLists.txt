cmake_minimum_required(VERSION 3.5)

project(ThreePowers VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

file(GLOB_RECURSE PROJECT_SOURCES CONFIGURE_DEPENDS
    main.cpp
    mainwindow.cpp mainwindow.h mainwindow.ui
    ui/*.cpp
    ui/*.h
    ui/*.ui
    gameloop.cpp gameloop.h
    player.cpp player.h
    action.cpp action.h
    location.cpp location.h
    skill.cpp skill.h
    world.cpp world.h
    item.cpp item.h
    requirement.h
    reserve.cpp reserve.h
    effect.cpp effect.h
    skillmodel.h skillmodel.cpp
    skilldelegate.h skilldelegate.cpp
    actionqueuemodel.h actionqueuemodel.cpp
    actionqueuedelegate.h actionqueuedelegate.cpp
    tooltip.cpp tooltip.h
    travelaction.cpp travelaction.h
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    find_package(Qt6 REQUIRED COMPONENTS Widgets)
find_package(Qt6 REQUIRED COMPONENTS Widgets)
find_package(Qt6 REQUIRED COMPONENTS Core)

    qt_add_executable(ThreePowers
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET ThreePowers APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(ThreePowers SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(ThreePowers
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(ThreePowers PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
target_link_libraries(ThreePowers PRIVATE Qt6::Widgets)
target_link_libraries(ThreePowers PRIVATE Qt6::Widgets)
target_link_libraries(ThreePowers PRIVATE Qt6::Core)
target_link_libraries(ThreePowers PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

target_include_directories(ThreePowers PRIVATE
    ${CMAKE_SOURCE_DIR}
    ui
)

file(COPY ${CMAKE_SOURCE_DIR}/config DESTINATION ${CMAKE_BINARY_DIR})

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.ThreePowers)
endif()
set_target_properties(ThreePowers PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS ThreePowers
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(ThreePowers)
endif()
