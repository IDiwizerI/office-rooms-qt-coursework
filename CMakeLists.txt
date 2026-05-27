cmake_minimum_required(VERSION 3.20)

project(office_rooms_qt_coursework
    VERSION 0.1.0
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

qt_standard_project_setup()

qt_add_executable(office_rooms_qt_coursework
    src/main.cpp
    resources/resources.qrc

    include/core/room.h
    include/documents/roomdocument.h
    include/models/roomtablemodel.h
    include/models/roomsortfilterproxymodel.h
    include/storage/roomfilestorage.h
    include/dialogs/roomdialog.h
    include/dialogs/aboutdialog.h
    include/dialogs/chartdialog.h
    include/services/printservice.h
    include/services/settingsmanager.h
    include/services/translationmanager.h
    include/widgets/roomdocumentwidget.h
)

target_include_directories(office_rooms_qt_coursework
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(office_rooms_qt_coursework
    PRIVATE
        Qt6::Widgets
)
