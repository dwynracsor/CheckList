#include "controllers/TaskController.h"
#include "domain/Task.h"
#include "domain/ChecklistItem.h"
#include "domain/Correction.h"
#include <iostream>
#include <cassert>

using namespace checklist;

int main() {
    std::cout << "=== TaskController Test ===" << std::endl;

    // Create controller
    auto controller = std::make_shared<TaskController>();

    // Test add task
    std::cout << "\n--- Add Task ---" << std::endl;
    auto task1 = controller->addTask("Buy groceries", "Personal");
    assert(task1 != nullptr);
    assert(controller->taskCount() == 1);
    assert(controller->getTask(0)->name() == "Buy groceries");
    assert(controller->getTask(0)->category() == "Personal");
    std::cout << "  Task 1 added: " << task1->name() << " [" << task1->category() << "]" << std::endl;

    auto task2 = controller->addTask("Fix bug", "Work");
    assert(task2 != nullptr);
    assert(controller->taskCount() == 2);
    std::cout << "  Task 2 added: " << task2->name() << " [" << task2->category() << "]" << std::endl;

    // Test canUndo
    assert(controller->canUndo());
    std::cout << "  Can undo: " << (controller->canUndo() ? "yes" : "no") << std::endl;

    // Test add item to task
    std::cout << "\n--- Add Item ---" << std::endl;
    controller->addItemToTask(0, "Milk");
    controller->addItemToTask(0, "Bread");
    assert(controller->getTask(0)->itemCount() == 2);
    std::cout << "  Task 0 items: " << controller->getTask(0)->itemCount() << std::endl;

    // Test toggle item
    std::cout << "\n--- Toggle Item ---" << std::endl;
    controller->toggleItemInTask(0, 0, true);
    assert(controller->getTask(0)->getItem(0)->done() == true);
    std::cout << "  Item 0 done: " << (controller->getTask(0)->getItem(0)->done() ? "yes" : "no") << std::endl;

    // Test isFinished (not all done)
    assert(controller->getTask(0)->isFinished() == false);
    std::cout << "  Is finished (not all done): " << (controller->getTask(0)->isFinished() ? "yes" : "no") << std::endl;

    controller->toggleItemInTask(0, 1, true);
    assert(controller->getTask(0)->isFinished() == true);
    std::cout << "  Is finished (all done): " << (controller->getTask(0)->isFinished() ? "yes" : "no") << std::endl;

    // Test undo - undoes the last toggle (item 1)
    std::cout << "\n--- Undo ---" << std::endl;
    controller->undo();
    assert(controller->getTask(0)->getItem(1)->done() == false);
    assert(controller->getTask(0)->getItem(0)->done() == true); // item 0 should still be done
    std::cout << "  After undo, item 1 done: " << (controller->getTask(0)->getItem(1)->done() ? "yes" : "no") << std::endl;
    std::cout << "  After undo, item 0 done: " << (controller->getTask(0)->getItem(0)->done() ? "yes" : "no") << std::endl;

    // Test redo - redoes the toggle of item 1
    std::cout << "\n--- Redo ---" << std::endl;
    controller->redo();
    assert(controller->getTask(0)->getItem(1)->done() == true);
    std::cout << "  After redo, item 1 done: " << (controller->getTask(0)->getItem(1)->done() ? "yes" : "no") << std::endl;

    // Test add correction
    std::cout << "\n--- Add Correction ---" << std::endl;
    auto corr = controller->addCorrectionToTask(1, "Urgent fix");
    assert(corr != nullptr);
    assert(controller->getTask(1)->correctionCount() == 1);
    std::cout << "  Correction added: " << corr->name() << std::endl;

    // Test add item to correction
    controller->addCorrectionToTask(1, "Second correction"); // just to test multiple
    controller->getTask(1)->getCorrection(0)->addNewItem("Review code");
    assert(controller->getTask(1)->getCorrection(0)->itemCount() == 1);
    assert(controller->getTask(1)->getCorrection(0)->getItem(0)->text() == "Review code");
    std::cout << "  Correction item 0 text: " << controller->getTask(1)->getCorrection(0)->getItem(0)->text() << std::endl;

    // Test set task name
    std::cout << "\n--- Rename Task ---" << std::endl;
    controller->setTaskName(0, "Weekly shopping");
    assert(controller->getTask(0)->name() == "Weekly shopping");
    std::cout << "  Task 0 new name: " << controller->getTask(0)->name() << std::endl;

    // Test set task category
    controller->setTaskCategory(0, "Home");
    assert(controller->getTask(0)->category() == "Home");
    std::cout << "  Task 0 new category: " << controller->getTask(0)->category() << std::endl;

    // Test remove task
    std::cout << "\n--- Remove Task ---" << std::endl;
    controller->removeTask(1);
    assert(controller->taskCount() == 1);
    assert(controller->getTask(0)->name() == "Weekly shopping");
    std::cout << "  Tasks after removal: " << controller->taskCount() << std::endl;

    // Test undo removal
    controller->undo();
    assert(controller->taskCount() == 2);
    std::cout << "  Tasks after undo: " << controller->taskCount() << std::endl;

    // Test canRedo after undo
    assert(controller->canRedo());
    std::cout << "  Can redo: " << (controller->canRedo() ? "yes" : "no") << std::endl;

    // Test clear history
    controller->clearHistory();
    assert(!controller->canUndo());
    assert(!controller->canRedo());
    std::cout << "  After clear: canUndo=" << (controller->canUndo() ? "yes" : "no")
              << ", canRedo=" << (controller->canRedo() ? "yes" : "no") << std::endl;

    // Test save
    std::cout << "\n--- Save ---" << std::endl;
    bool saved = controller->save();
    std::cout << "  Save result: " << (saved ? "OK" : "FAILED") << std::endl;

    std::cout << "\n=== All TaskController tests passed! ===" << std::endl;
    return 0;
}