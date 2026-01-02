cmake_minimum_required(VERSION 3.5)
project(ThreePowers VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Core)

set(PROJECT_SOURCES
    main.cpp
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
    travelaction.cpp travelaction.h
    logger.h
    # UI files
    ui/skillmodel.cpp ui/skillmodel.h
    ui/skilldelegate.cpp ui/skilldelegate.h
    ui/actionqueuemodel.cpp ui/actionqueuemodel.h
    ui/actionqueuedelegate.cpp ui/actionqueuedelegate.h
    ui/actionbutton.cpp ui/actionbutton.h
    ui/tooltip.cpp ui/tooltip.h
    ui/messagelog.cpp ui/messagelog.h
    ui/reservemodel.cpp ui/reservemodel.h
    ui/reservedelegate.cpp ui/reservedelegate.h
    ui/mainwindow.cpp ui/mainwindow.h
    ui/mainwindow.ui
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(ThreePowers
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        ui/menubuilder.h ui/menubuilder.cpp
        ui/numberspinnerwidget.h ui/numberspinnerwidget.cpp
        trigger.h trigger.cpp
        triggermanager.h triggermanager.cpp
        event.h event.cpp
    )
else()
    if(ANDROID)
        add_library(ThreePowers SHARED
            ${PROJECT_SOURCES}
        )
    else()
        add_executable(ThreePowers
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(ThreePowers PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Core)

target_include_directories(ThreePowers PRIVATE
    ${CMAKE_SOURCE_DIR}
    ui
)

add_custom_target(copy_config ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/config
            ${CMAKE_BINARY_DIR}/config
    COMMENT "Copying config directory"
)
add_dependencies(ThreePowers copy_config)

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
