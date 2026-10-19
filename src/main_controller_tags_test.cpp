#include "controllers/TaskController.h"
#include "domain/Task.h"
#include <iostream>
#include <cassert>
#include <memory>

using namespace checklist;

void testAddRemoveTag() {
    std::cout << "Test: Add/Remove tag via controller..." << std::endl;
    auto controller = std::make_shared<TaskController>();

    auto task = controller->addTask("Tarea con tags", "Personal");

    controller->addTaskTag(0, "trabajo");
    assert(controller->getTaskTagCount(0) == 1);
    assert(controller->taskHasTag(0, "trabajo"));

    controller->addTaskTag(0, "urgente");
    assert(controller->getTaskTagCount(0) == 2);
    assert(controller->taskHasTag(0, "urgente"));

    controller->removeTaskTag(0, "trabajo");
    assert(controller->getTaskTagCount(0) == 1);
    assert(!controller->taskHasTag(0, "trabajo"));

    controller->clearTaskTags(0);
    assert(controller->getTaskTagCount(0) == 0);

    std::cout << "  OK" << std::endl;
}

void testSetTags() {
    std::cout << "Test: Set tags via controller..." << std::endl;
    auto controller = std::make_shared<TaskController>();

    auto task = controller->addTask("Tarea", "Personal");

    std::vector<std::string> tags = {"tag1", "tag2"};
    controller->setTaskTags(0, tags);

    const auto& retrievedTags = controller->getTaskTags(0);
    assert(retrievedTags.size() == 2);
    assert(retrievedTags[0] == "tag1");
    assert(retrievedTags[1] == "tag2");

    std::cout << "  OK" << std::endl;
}

void testGetAllTags() {
    std::cout << "Test: Get all tags..." << std::endl;
    auto controller = std::make_shared<TaskController>();

    auto t1 = controller->addTask("Tarea 1", "Personal");
    auto t2 = controller->addTask("Tarea 2", "Personal");

    controller->addTaskTag(0, "trabajo");
    controller->addTaskTag(0, "urgente");
    controller->addTaskTag(1, "trabajo");
    controller->addTaskTag(1, "casa");

    auto allTags = controller->getAllTags();
    assert(allTags.size() == 3); // trabajo, urgente, casa
    assert(controller->taskHasTag(0, "urgente"));
    assert(controller->taskHasTag(1, "casa"));

    std::cout << "  OK" << std::endl;
}

int main() {
    std::cout << "=== TaskController Tags Tests ===" << std::endl;

    testAddRemoveTag();
    testSetTags();
    testGetAllTags();

    std::cout << "\n=== All TaskController tags tests passed! ===" << std::endl;
    return 0;
}