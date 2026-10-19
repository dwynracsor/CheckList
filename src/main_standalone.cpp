#include "controllers/TaskController.h"
#include "domain/Task.h"
#include "domain/ChecklistItem.h"
#include "domain/Correction.h"
#include "persistence/JsonStorage.h"
#include <iostream>
#include <string>
#include <iomanip>

using namespace checklist;

void printMenu() {
    std::cout << "\n=== Checklist Application ===" << std::endl;
    std::cout << "1. List tasks" << std::endl;
    std::cout << "2. Add task" << std::endl;
    std::cout << "3. Add item to task" << std::endl;
    std::cout << "4. Toggle item" << std::endl;
    std::cout << "5. Remove task" << std::endl;
    std::cout << "6. Undo" << std::endl;
    std::cout << "7. Redo" << std::endl;
    std::cout << "8. Save" << std::endl;
    std::cout << "9. Load" << std::endl;
    std::cout << "0. Exit" << std::endl;
    std::cout << "Choose option: ";
}

void listTasks(const TaskController& controller) {
    std::cout << "\n--- Tasks (" << controller.taskCount() << ") ---" << std::endl;
    for (size_t i = 0; i < controller.taskCount(); ++i) {
        auto task = controller.getTask(static_cast<int>(i));
        if (!task) continue;
        std::cout << "  [" << i << "] " << task->name()
                  << " | Category: " << task->category()
                  << " | Date: " << task->date().toString()
                  << " | Items: " << task->itemCount()
                  << " | Finished: " << (task->isFinished() ? "yes" : "no")
                  << std::endl;
        for (size_t j = 0; j < task->itemCount(); ++j) {
            auto item = task->getItem(j);
            if (item) {
                std::cout << "      - [" << (item->done() ? "X" : " ") << "] "
                          << item->text() << std::endl;
            }
        }
        for (size_t j = 0; j < task->correctionCount(); ++j) {
            auto corr = task->getCorrection(j);
            if (corr) {
                std::cout << "      Correction: " << corr->name()
                          << " | Items: " << corr->itemCount() << std::endl;
            }
        }
    }
}

int main() {
    std::cout << "Checklist Application (Standalone Mode)" << std::endl;

    auto controller = std::make_shared<TaskController>();
    controller->load();

    int option = -1;
    while (option != 0) {
        printMenu();
        std::cin >> option;
        std::cin.ignore();

        switch (option) {
            case 1:
                listTasks(*controller);
                break;

            case 2: {
                std::string name, category;
                std::cout << "Task name: ";
                std::getline(std::cin, name);
                std::cout << "Category (optional): ";
                std::getline(std::cin, category);
                controller->addTask(name, category);
                break;
            }

            case 3: {
                int taskIndex;
                std::string text;
                std::cout << "Task index: ";
                std::cin >> taskIndex;
                std::cin.ignore();
                std::cout << "Item text: ";
                std::getline(std::cin, text);
                controller->addItemToTask(taskIndex, text);
                break;
            }

            case 4: {
                int taskIndex, itemIndex;
                bool done;
                std::cout << "Task index: ";
                std::cin >> taskIndex;
                std::cout << "Item index: ";
                std::cin >> itemIndex;
                std::cout << "Done (0/1): ";
                std::cin >> done;
                controller->toggleItemInTask(taskIndex, itemIndex, done);
                break;
            }

            case 5: {
                int taskIndex;
                std::cout << "Task index: ";
                std::cin >> taskIndex;
                controller->removeTask(taskIndex);
                break;
            }

            case 6:
                controller->undo();
                std::cout << "Undo performed." << std::endl;
                break;

            case 7:
                controller->redo();
                std::cout << "Redo performed." << std::endl;
                break;

            case 8: {
                bool saved = controller->save();
                std::cout << "Save " << (saved ? "successful" : "failed") << std::endl;
                break;
            }

            case 9:
                controller->load();
                std::cout << "Tasks loaded." << std::endl;
                break;

            case 0:
                std::cout << "Goodbye!" << std::endl;
                break;

            default:
                std::cout << "Invalid option." << std::endl;
        }
    }

    return 0;
}