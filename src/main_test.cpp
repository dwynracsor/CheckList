#include "domain/Task.h"
#include "domain/ChecklistItem.h"
#include "domain/Correction.h"
#include <iostream>
#include <chrono>

using namespace checklist;

void testChecklistItem() {
    std::cout << "Testing ChecklistItem..." << std::endl;

    // Create item
    auto item = std::make_shared<ChecklistItem>("Buy milk", false);
    std::cout << "  Item created: '" << item->text() << "' done=" << item->done() << std::endl;

    // Toggle
    item->toggle();
    std::cout << "  After toggle: done=" << item->done() << std::endl;

    // Test callbacks
    bool callbackCalled = false;
    item->setTextChangedCallback([&]() { callbackCalled = true; std::cout << "    TextChanged callback called!" << std::endl; });
    item->setDoneChangedCallback([&]() { callbackCalled = true; std::cout << "    DoneChanged callback called!" << std::endl; });

    item->setText("Buy eggs");
    if (callbackCalled) std::cout << "    Text change callback worked!" << std::endl;

    item->toggle();
    if (callbackCalled) std::cout << "    Done toggle callback worked!" << std::endl;

    // Test copy/move
    auto item2 = std::make_shared<ChecklistItem>(*item);
    std::cout << "  Copy constructed: '" << item2->text() << "' done=" << item2->done() << std::endl;

    auto item3 = std::move(item2);
    std::cout << "  Moved: items_ size = " << item3->text() << std::endl;

    std::cout << "  ChecklistItem tests passed!" << std::endl;
}

void testTask() {
    std::cout << "\nTesting Task..." << std::endl;

    // Create task with date
    Date dueDate(2024, 12, 25);
    auto task = std::make_shared<Task>("Weekly shopping", dueDate);

    std::cout << "  Task ID: " << task->id() << std::endl;
    std::cout << "  Task name: " << task->name() << std::endl;
    std::cout << "  Task date: " << task->date().toString() << std::endl;
    std::cout << "  Is finished: " << (task->isFinished() ? "yes" : "no") << std::endl;

    // Add items
    auto item1 = std::make_shared<ChecklistItem>("Buy milk", false);
    auto item2 = std::make_shared<ChecklistItem>("Buy bread", false);
    task->addItem(item1);
    task->addItem(item2);

    std::cout << "  Item count: " << task->itemCount() << std::endl;
    std::cout << "  Is finished (not all done): " << (task->isFinished() ? "yes" : "no") << std::endl;

    // Mark all done
    item1->setDone(true);
    item2->setDone(true);
    std::cout << "  Is finished (all done): " << (task->isFinished() ? "yes" : "no") << std::endl;

    // Test callbacks
    bool nameChanged = false;
    task->setNameChangedCallback([&]() { nameChanged = true; std::cout << "    NameChanged callback called!" << std::endl; });

    task->setName("Weekly grocery shopping");
    if (nameChanged) std::cout << "    Name change callback worked!" << std::endl;

    // Test category
    task->setCategory("Work");
    std::cout << "  Category: " << task->category() << std::endl;

    // Test tokenize
    auto tokens = Task::tokenize("hello world test");
    std::cout << "  Tokenize [hello world test]: ";
    for (const auto& t : tokens) std::cout << t << " ";
    std::cout << std::endl;

    // Test copy/move
    auto task2 = std::make_shared<Task>(*task);
    std::cout << "  Copy: name = " << task2->name() << std::endl;

    auto task3 = std::move(task2);
    std::cout << "  Moved successfully" << std::endl;

    std::cout << "  Task tests passed!" << std::endl;
}

void testCorrection() {
    std::cout << "\nTesting Correction..." << std::endl;

    auto corr = std::make_shared<Correction>("Priority items", Date(2024, 1, 15));

    // Add items
    corr->addNewItem("Buy milk");
    corr->addNewItem("Call doctor");
    std::cout << "  Item count after 2 adds: " << corr->itemCount() << std::endl;

    // Toggle item
    corr->toggleItem(0);
    std::cout << "  After toggle item 0 done: " << corr->getItem(0)->done() << std::endl;

    // Test callbacks
    bool nameChanged = false;
    corr->setNameChangedCallback([&]() { nameChanged = true; std::cout << "    NameChanged callback called!" << std::endl; });

    corr->setName("High priority items");
    if (nameChanged) std::cout << "    Name change callback worked!" << std::endl;

    // Test copy/move
    auto corr2 = std::make_shared<Correction>(*corr);
    std::cout << "  Copy: name = " << corr2->name() << ", items = " << corr2->itemCount() << std::endl;

    auto corr3 = std::move(corr2);
    std::cout << "  Moved successfully" << std::endl;

    // Test tokenize
    auto tokens = Correction::tokenize("urgent meeting today");
    std::cout << "  Tokenize [urgent meeting today]: ";
    for (const auto& t : tokens) std::cout << t << " ";
    std::cout << std::endl;

    std::cout << "  Correction tests passed!" << std::endl;
}

int main() {
    std::cout << "=== Checklist Domain Models Test ===" << std::endl;

    testChecklistItem();
    testTask();
    testCorrection();

    std::cout << "\n=== All tests passed! ===" << std::endl;
    return 0;
}