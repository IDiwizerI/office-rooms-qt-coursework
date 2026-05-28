cmake_minimum_required(VERSION 3.20)

project(office_rooms_qt_coursework
    VERSION 0.1.0
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Qt6 REQUIRED COMPONENTS Widgets PrintSupport Charts LinguistTools)

qt_standard_project_setup()

set(PROJECT_SOURCES
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

set(TRANSLATION_FILES
    translations/office_rooms_en.ts
    translations/office_rooms_ru.ts
    translations/office_rooms_de.ts
)

qt_add_executable(office_rooms_qt_coursework
    ${PROJECT_SOURCES}
)

qt_add_translations(office_rooms_qt_coursework
    TS_FILES ${TRANSLATION_FILES}
    SOURCES ${PROJECT_SOURCES}
    RESOURCE_PREFIX "/translations"
    LUPDATE_OPTIONS -no-obsolete
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
