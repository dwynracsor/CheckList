#include "domain/Task.h"
#include "domain/ChecklistItem.h"
#include "domain/Correction.h"
#include "persistence/JsonStorage.h"
#include <iostream>
#include <fstream>

using namespace checklist;

int main() {
    std::cout << "=== JsonStorage Test ===" << std::endl;

    // Create storage with test path
    JsonStorage storage("./test_tasks.json");

    // Create some tasks
    auto task1 = std::make_shared<Task>("Buy groceries", Date(2024, 12, 25));
    task1->setCategory("Personal");
    task1->addItem(std::make_shared<ChecklistItem>("Milk", false));
    task1->addItem(std::make_shared<ChecklistItem>("Bread", true));

    auto task2 = std::make_shared<Task>("Fix bug #42", Date(2024, 12, 26));
    task2->setCategory("Work");
    task2->addItem(std::make_shared<ChecklistItem>("Investigate", false));
    task2->addItem(std::make_shared<ChecklistItem>("Fix", false));
    task2->addItem(std::make_shared<ChecklistItem>("Test", false));

    // Add a correction to task2
    auto correction = std::make_shared<Correction>("Urgent fix", Date(2024, 12, 26));
    correction->addNewItem("Review code");
    correction->addNewItem("Deploy");
    task2->addCorrection(correction);

    std::vector<std::shared_ptr<Task>> tasks = {task1, task2};

    // Save
    std::cout << "Saving tasks..." << std::endl;
    bool saved = storage.save(tasks);
    std::cout << "Save result: " << (saved ? "OK" : "FAILED") << std::endl;

    // Load
    std::cout << "\nLoading tasks..." << std::endl;
    auto loaded = storage.load();
    std::cout << "Loaded " << loaded.size() << " tasks" << std::endl;

    // Verify
    bool ok = true;
    if (loaded.size() != 2) {
        std::cout << "FAIL: Expected 2 tasks, got " << loaded.size() << std::endl;
        ok = false;
    }

    if (loaded[0]->name() != "Buy groceries") {
        std::cout << "FAIL: Task 0 name mismatch" << std::endl;
        ok = false;
    }

    if (loaded[0]->category() != "Personal") {
        std::cout << "FAIL: Task 0 category mismatch" << std::endl;
        ok = false;
    }

    if (loaded[0]->itemCount() != 2) {
        std::cout << "FAIL: Task 0 item count mismatch" << std::endl;
        ok = false;
    }

    if (loaded[1]->name() != "Fix bug #42") {
        std::cout << "FAIL: Task 1 name mismatch" << std::endl;
        ok = false;
    }

    if (loaded[1]->correctionCount() != 1) {
        std::cout << "FAIL: Task 1 correction count mismatch" << std::endl;
        ok = false;
    }

    if (loaded[1]->getCorrection(0)->itemCount() != 2) {
        std::cout << "FAIL: Correction item count mismatch" << std::endl;
        ok = false;
    }

    // Show JSON content
    std::ifstream file("./test_tasks.json");
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    std::cout << "\nJSON content:\n" << content << std::endl;

    if (ok) {
        std::cout << "\n=== All JsonStorage tests passed! ===" << std::endl;
        return 0;
    } else {
        std::cout << "\n=== Some tests FAILED ===" << std::endl;
        return 1;
    }
}