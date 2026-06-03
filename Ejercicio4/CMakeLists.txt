cmake_minimum_required(VERSION 3.16)
project(KanbanBoard VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# ── Qt version detection ──────────────────────────────────────────────────────
find_package(Qt6 QUIET COMPONENTS Core Gui Widgets Network)
if(Qt6_FOUND)
    set(QT Qt6)
    message(STATUS "Using Qt6")
else()
    find_package(Qt5 REQUIRED COMPONENTS Core Gui Widgets Network)
    set(QT Qt5)
    message(STATUS "Using Qt5")
endif()

# ── Sources ───────────────────────────────────────────────────────────────────
set(SOURCES
    src/main.cpp
    src/apiclient.cpp
    src/logindialog.cpp
    src/mainwindow.cpp
    src/kanbanboard.cpp
    src/columnwidget.cpp
    src/cardwidget.cpp
    src/carddialog.cpp
)

set(HEADERS
    src/models.h
    src/apiclient.h
    src/logindialog.h
    src/mainwindow.h
    src/kanbanboard.h
    src/columnwidget.h
    src/cardwidget.h
    src/carddialog.h
)

add_executable(KanbanBoard ${SOURCES} ${HEADERS})

target_link_libraries(KanbanBoard PRIVATE
    ${QT}::Core
    ${QT}::Gui
    ${QT}::Widgets
    ${QT}::Network
)

target_include_directories(KanbanBoard PRIVATE src)
