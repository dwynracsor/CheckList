cmake_minimum_required(VERSION 3.21)

project(ChecklistApp VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Qt6 packages (Widgets para la UI, Qml requerido por el backend: QQmlListProperty)
find_package(Qt6 REQUIRED COMPONENTS Core Widgets Qml Svg)
find_package(Qt6 COMPONENTS Test QUIET)

# Sources: interfaz (src1) + backend existente (src)
set(SOURCES
    src1/main.cpp
    src1/mainwindow.cpp
    src1/listcard.cpp
    src1/itemrow.cpp
    src1/itemaddrow.cpp
    src1/toggleswitch.cpp
    # Backend
    src/domain/ChecklistItem.cpp
    src/domain/Correction.cpp
    src/domain/Task.cpp
    src/persistence/JsonStorage.cpp
    src/controllers/TaskController.cpp
    src/undo/UndoCommand.cpp
)

add_executable(${PROJECT_NAME} ${SOURCES})

target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt6::Core
    Qt6::Widgets
    Qt6::Qml
    Qt6::Svg
)

target_include_directories(${PROJECT_NAME} PRIVATE src src1)

# Windows specific
if(WIN32)
    set_target_properties(${PROJECT_NAME} PROPERTIES
        WIN32_EXECUTABLE TRUE
    )
endif()

# Tests
if(Qt6Test_FOUND)
    enable_testing()
    add_subdirectory(tests)
endif()
