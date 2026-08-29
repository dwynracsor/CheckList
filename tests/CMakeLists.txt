if(NOT Qt6Test_FOUND)
    return()
endif()

# Create a static library from domain/persistence sources for testing
add_library(ChecklistApp_lib STATIC
    ${CMAKE_SOURCE_DIR}/src/domain/ChecklistItem.cpp
    ${CMAKE_SOURCE_DIR}/src/domain/Correction.cpp
    ${CMAKE_SOURCE_DIR}/src/domain/Task.cpp
    ${CMAKE_SOURCE_DIR}/src/persistence/JsonStorage.cpp
    ${CMAKE_SOURCE_DIR}/src/controllers/TaskController.cpp
    ${CMAKE_SOURCE_DIR}/src/undo/UndoCommand.cpp
)

target_link_libraries(ChecklistApp_lib PUBLIC
    Qt6::Core
    Qt6::Qml
)

target_include_directories(ChecklistApp_lib PUBLIC
    ${CMAKE_SOURCE_DIR}/src
)

# Each test as a separate executable
add_executable(ChecklistItemTests unit/ChecklistItemTests.cpp)
target_link_libraries(ChecklistItemTests PRIVATE Qt6::Core Qt6::Test ChecklistApp_lib)
target_include_directories(ChecklistItemTests PRIVATE ${CMAKE_SOURCE_DIR}/src)
add_test(NAME ChecklistItemTests COMMAND ChecklistItemTests)

add_executable(CorrectionTests unit/CorrectionTests.cpp)
target_link_libraries(CorrectionTests PRIVATE Qt6::Core Qt6::Test ChecklistApp_lib)
target_include_directories(CorrectionTests PRIVATE ${CMAKE_SOURCE_DIR}/src)
add_test(NAME CorrectionTests COMMAND CorrectionTests)

add_executable(TaskTests unit/TaskTests.cpp)
target_link_libraries(TaskTests PRIVATE Qt6::Core Qt6::Test ChecklistApp_lib)
target_include_directories(TaskTests PRIVATE ${CMAKE_SOURCE_DIR}/src)
add_test(NAME TaskTests COMMAND TaskTests)

add_executable(JsonStorageTests unit/JsonStorageTests.cpp)
target_link_libraries(JsonStorageTests PRIVATE Qt6::Core Qt6::Test ChecklistApp_lib)
target_include_directories(JsonStorageTests PRIVATE ${CMAKE_SOURCE_DIR}/src)
add_test(NAME JsonStorageTests COMMAND JsonStorageTests)

add_executable(TaskControllerTests unit/TaskControllerTests.cpp)
target_link_libraries(TaskControllerTests PRIVATE Qt6::Core Qt6::Test ChecklistApp_lib)
target_include_directories(TaskControllerTests PRIVATE ${CMAKE_SOURCE_DIR}/src)
add_test(NAME TaskControllerTests COMMAND TaskControllerTests)
