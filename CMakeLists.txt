cmake_minimum_required(VERSION 3.21)

project(ChecklistApp VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Qt6 packages
find_package(Qt6 REQUIRED COMPONENTS Core Quick Qml QuickControls2)
find_package(Qt6 COMPONENTS Test QUIET)

# Sources
set(SOURCES
    src/main.cpp
    # Domain models
    src/domain/ChecklistItem.cpp
    src/domain/Correction.cpp
    src/domain/Task.cpp
    # Persistence
    src/persistence/JsonStorage.cpp
    # Controllers
    src/controllers/TaskController.cpp
    # Undo
    src/undo/UndoCommand.cpp
)

set(HEADERS
    src/domain/ChecklistItem.h
    src/domain/Correction.h
    src/domain/Task.h
    src/persistence/JsonStorage.h
    src/controllers/TaskController.h
    src/undo/UndoCommand.h
)

# Resources
set(RESOURCES
    resources/resources.qrc
)

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS} ${RESOURCES})

target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt6::Core
    Qt6::Quick
    Qt6::Qml
    Qt6::QuickControls2
)

target_include_directories(${PROJECT_NAME} PRIVATE src)

# QML module registration
qt6_add_qml_module(${PROJECT_NAME}
    URI ChecklistApp
    VERSION 1.0
    QML_FILES
        qml/Main.qml
        qml/AppWindow.qml
        qml/styles/Theme.qml
        qml/styles/ButtonStyle.qml
        qml/styles/PrimaryButton.qml
        qml/styles/SuccessButton.qml
        qml/styles/DangerButton.qml
        qml/styles/SmallButton.qml
        qml/styles/StyledTextField.qml
        qml/styles/StyledTextArea.qml
        qml/styles/StyledCheckBox.qml
        qml/components/TaskCard.qml
        qml/components/CorrectionBlock.qml
        qml/components/ChecklistDelegate.qml
        qml/components/InlineEditor.qml
        qml/components/AddItemRow.qml
        qml/components/ToggleFormButton.qml
        qml/components/TaskForm.qml
        qml/components/ChecklistBuilder.qml
        qml/components/EmptyState.qml
        qml/components/SyncStatus.qml
        qml/components/ConfirmationDialog.qml
    RESOURCES
        resources/icons/app.svg
)



# Tests
if(Qt6Test_FOUND)
    enable_testing()
    add_subdirectory(tests)
endif()

# Install rules
install(TARGETS ${PROJECT_NAME}
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Windows specific
if(WIN32)
    set_target_properties(${PROJECT_NAME} PROPERTIES
        WIN32_EXECUTABLE TRUE
    )
    
    # Windows icon (requires .ico file)
    # set(APP_ICON_RESOURCE "${CMAKE_CURRENT_SOURCE_DIR}/resources/icons/app.rc")
    # if(EXISTS "${APP_ICON_RESOURCE}")
    #     target_sources(${PROJECT_NAME} PRIVATE ${APP_ICON_RESOURCE})
    # endif()
endif()

# macOS specific
if(APPLE)
    set(MACOSX_BUNDLE_ICON_FILE "app.icns")
    set(MACOSX_BUNDLE_GUI_IDENTIFIER "com.checklistapp.desktop")
    set(MACOSX_BUNDLE_LONG_VERSION_STRING "${PROJECT_VERSION}")
    set(MACOSX_BUNDLE_BUNDLE_NAME "${PROJECT_NAME}")
    set(MACOSX_BUNDLE_BUNDLE_VERSION "${PROJECT_VERSION}")
    set(MACOSX_BUNDLE_SHORT_VERSION_STRING "${PROJECT_VERSION}")
endif()

# Linux specific
if(UNIX AND NOT APPLE)
    # Install desktop file
    install(FILES deploy/linux/checklistapp.desktop
        DESTINATION share/applications
    )
    
    # Install icon
    install(FILES resources/icons/app.svg
        DESTINATION share/icons/hicolor/scalable/apps
        RENAME checklistapp.svg
    )
endif()
