#include "domain/Task.h"
#include <iostream>
#include <cassert>
#include <memory>

using namespace checklist;

void testAddRemoveTag() {
    std::cout << "Test: Add/Remove tag..." << std::endl;
    auto task = std::make_shared<Task>("Tarea con tags", Date(2024, 12, 25));
    
    assert(task->tagCount() == 0);
    
    task->addTag("trabajo");
    assert(task->tagCount() == 1);
    assert(task->hasTag("trabajo"));
    
    task->addTag("urgente");
    task->addTag("casa");
    assert(task->tagCount() == 3);
    assert(task->hasTag("urgente"));
    assert(task->hasTag("casa"));
    
    // Duplicado no se agrega
    task->addTag("trabajo");
    assert(task->tagCount() == 3);
    
    // Vacío no se agrega
    task->addTag("");
    assert(task->tagCount() == 3);
    
    task->removeTag("urgente");
    assert(task->tagCount() == 2);
    assert(!task->hasTag("urgente"));
    
    task->clearTags();
    assert(task->tagCount() == 0);
    assert(!task->hasTag("trabajo"));
    
    std::cout << "  OK" << std::endl;
}

void testSetTags() {
    std::cout << "Test: Set tags..." << std::endl;
    auto task = std::make_shared<Task>("Tarea", Date(2024, 12, 25));
    
    std::vector<std::string> newTags = {"tag1", "tag2", "tag3"};
    task->setTags(newTags);
    
    assert(task->tagCount() == 3);
    assert(task->hasTag("tag1"));
    assert(task->hasTag("tag2"));
    assert(task->hasTag("tag3"));
    
    // Set same tags - no change
    task->setTags(newTags);
    assert(task->tagCount() == 3);
    
    std::cout << "  OK" << std::endl;
}

void testTagsCallback() {
    std::cout << "Test: Tags callback..." << std::endl;
    auto task = std::make_shared<Task>("Tarea", Date(2024, 12, 25));
    
    bool callbackCalled = false;
    task->setTagsChangedCallback([&]() { callbackCalled = true; });
    
    task->addTag("test");
    assert(callbackCalled);
    
    callbackCalled = false;
    task->removeTag("test");
    assert(callbackCalled);
    
    std::cout << "  OK" << std::endl;
}

void testTagsCopyMove() {
    std::cout << "Test: Copy/move with tags..." << std::endl;
    auto task1 = std::make_shared<Task>("Original", Date(2024, 12, 25));
    task1->addTag("tag1");
    task1->addTag("tag2");
    
    // Copy
    auto task2 = std::make_shared<Task>(*task1);
    assert(task2->tagCount() == 2);
    assert(task2->hasTag("tag1"));
    assert(task2->hasTag("tag2"));
    
    // Move
    auto task3 = std::move(task2);
    assert(task3->tagCount() == 2);
    assert(task3->hasTag("tag1"));
    assert(task3->hasTag("tag2"));
    
    std::cout << "  OK" << std::endl;
}

int main() {
    std::cout << "=== Task Tags Tests ===" << std::endl;
    
    testAddRemoveTag();
    testSetTags();
    testTagsCallback();
    testTagsCopyMove();
    
    std::cout << "\n=== All tag tests passed! ===" << std::endl;
    return 0;
}