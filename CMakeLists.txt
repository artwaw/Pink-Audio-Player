cmake_minimum_required(VERSION 3.5)

project(PAP VERSION 0.10 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 COMPONENTS Widgets Gui Multimedia Sql LinguistTools REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets Gui Multimedia Sql LinguistTools REQUIRED)

set(TS_FILES
    PAP_en_GB.ts
)

set(papIcon ${CMAKE_CURRENT_SOURCE_DIR}/img/pap.icns)
set_source_files_properties(${papIcon} PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")

set(DIST_FILES README.md LICENSE)

set(PROJECT_SOURCES
        main.cpp res.qrc
        Player.cpp Player.h Player.ui
        ${TS_FILES}
        ${DIST_FILES}
)

qt_add_executable(PAP
    MACOSX_BUNDLE
    MANUAL_FINALIZATION
    ${papIcon}
    ${PROJECT_SOURCES}
)

qt_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})

target_link_libraries(PAP PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
target_link_libraries(PAP PRIVATE Qt${QT_VERSION_MAJOR}::Gui)
target_link_libraries(PAP PRIVATE Qt${QT_VERSION_MAJOR}::Multimedia)
target_link_libraries(PAP PRIVATE Qt${QT_VERSION_MAJOR}::Sql)

set_target_properties(PAP PROPERTIES
    MACOSX_BUNDLE TRUE
    MACOSX_BUNDLE_GUI_IDENTIFIER PAP.TrollNet.com.pl
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE_ICON_FILE pap.icns
    MACOSX_BUNDLE_COPYRIGHT "(c)2021 Artur Wawrowski"
    OUTPUT_NAME "Pink Audio Player"
)

qt_finalize_executable(PAP)

