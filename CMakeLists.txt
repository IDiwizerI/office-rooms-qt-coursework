cmake_minimum_required(VERSION 3.20)

project(office_rooms_qt_coursework
    VERSION 0.1.0
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Qt6 REQUIRED COMPONENTS Widgets PrintSupport Charts)

qt_standard_project_setup()

qt_add_executable(office_rooms_qt_coursework
    src/main.cpp
    resources/resources.qrc

    src/model/RoomTableModel.h
    src/model/RoomTableModel.cpp
    src/model/RoomSortFilterProxyModel.h
    src/model/RoomSortFilterProxyModel.cpp

    src/services/SettingsManager.h
    src/services/SettingsManager.cpp
    src/services/TranslationManager.h
    src/services/TranslationManager.cpp
    src/services/PrintService.h
    src/services/PrintService.cpp

    src/ui/MainWindow.h
    src/ui/MainWindow.cpp
    src/ui/MainWindow.ui
    src/ui/DocumentWidget.h
    src/ui/DocumentWidget.cpp
    src/ui/DocumentWidget.ui
    src/ui/RoomEditDialog.h
    src/ui/RoomEditDialog.cpp
    src/ui/RoomEditDialog.ui
    src/ui/AboutDialog.h
    src/ui/AboutDialog.cpp
    src/ui/AboutDialog.ui
    src/ui/ChartDialog.h
    src/ui/ChartDialog.cpp
    src/ui/ChartDialog.ui

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
        ${CMAKE_CURRENT_SOURCE_DIR}/src/model
        ${CMAKE_CURRENT_SOURCE_DIR}/src/services
        ${CMAKE_CURRENT_SOURCE_DIR}/src/ui
)

target_link_libraries(office_rooms_qt_coursework
    PRIVATE
        Qt6::Widgets
        Qt6::PrintSupport
        Qt6::Charts
)
