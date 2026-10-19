cmake_minimum_required(VERSION 3.16)
project(checklist LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Source files
set(SOURCES
    src/main.cpp
    src/domain/Task.cpp
    src/domain/ChecklistItem.cpp
    src/domain/Correction.cpp
    src/domain/TaskTemplate.cpp
    src/domain/StatsCalculator.cpp
    src/controllers/TaskController.cpp
    src/controllers/TaskControllerQml.cpp
    src/controllers/TaskQml.cpp
    src/undo/UndoCommand.cpp
    src/persistence/JsonStorage.cpp
    src/sync/SyncManager.cpp
    src/ical/ICalExporter.cpp
    src/ical/ICalImporter.cpp
)

# Optional: Qt6 support
option(USE_QT "Build with Qt6 GUI" OFF)
if(USE_QT)
    find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Quick QuickControls2)

    # Qt resources
    qt_add_resources(qml_resources qml/qml.qrc)

    qt_add_executable(checklist
        ${SOURCES}
        ${qml_resources}
    )

    target_link_libraries(checklist PRIVATE Qt6::Widgets Qt6::Quick Qt6::QuickControls2)
else()
    add_executable(checklist-standalone ${SOURCES} src/main_standalone.cpp)
endif()

# Install
if(USE_QT)
    install(TARGETS checklist
        RUNTIME DESTINATION bin)
else()
    install(TARGETS checklist-standalone
        RUNTIME DESTINATION bin)
endif()