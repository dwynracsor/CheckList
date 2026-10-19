#include "controllers/TaskController.h"
#include "domain/Task.h"
#include <iostream>
#include <cassert>
#include <memory>

using namespace checklist;

void testSetAndGetDependency() {
    std::cout << "Test: Set/Get dependency..." << std::endl;
    auto controller = std::make_shared<TaskController>();

    auto task1 = controller->addTask("Tarea padre", "Personal");
    auto task2 = controller->addTask("Tarea hija", "Personal");

    // Establecer dependencia: task2 depende de task1
    controller->setTaskDependency(1, TaskDependency::MustCompleteBefore, task1->id());

    assert(controller->getTaskDependencyType(1) == TaskDependency::MustCompleteBefore);
    assert(controller->getTaskDependencyId(1) == task1->id());
    assert(controller->isTaskBlockedByDependency(1)); // task1 no está terminada

    // Marcar task1 como terminada (todos sus ítems hechos)
    // task1 no tiene ítems, así que no puede estar "finished" según la lógica actual
    // Pero podemos verificar que findTaskIndexById funciona
    assert(controller->findTaskIndexById(task1->id()) == 0);
    assert(controller->findTaskIndexById(task2->id()) == 1);

    std::cout << "  OK" << std::endl;
}

void testRemoveDependency() {
    std::cout << "Test: Remove dependency..." << std::endl;
    auto controller = std::make_shared<TaskController>();

    auto task1 = controller->addTask("Padre", "Personal");
    auto task2 = controller->addTask("Hija", "Personal");

    controller->setTaskDependency(1, TaskDependency::MustCompleteBefore, task1->id());
    assert(controller->getTaskDependencyType(1) == TaskDependency::MustCompleteBefore);

    controller->removeTaskDependency(1);
    assert(controller->getTaskDependencyType(1) == TaskDependency::None);
    assert(controller->getTaskDependencyId(1) == "");
    assert(!controller->isTaskBlockedByDependency(1));

    std::cout << "  OK" << std::endl;
}

void testSelfDependencyRejected() {
    std::cout << "Test: Self dependency rejected..." << std::endl;
    auto controller = std::make_shared<TaskController>();

    auto task1 = controller->addTask("Tarea", "Personal");
    std::string errorBefore = controller->lastError();

    controller->setTaskDependency(0, TaskDependency::MustCompleteBefore, task1->id());

    assert(controller->lastError() != errorBefore);
    assert(controller->getTaskDependencyType(0) == TaskDependency::None);

    std::cout << "  OK" << std::endl;
}

void testNonexistentDependencyRejected() {
    std::cout << "Test: Nonexistent dependency rejected..." << std::endl;
    auto controller = std::make_shared<TaskController>();

    auto task1 = controller->addTask("Tarea", "Personal");
    std::string errorBefore = controller->lastError();

    controller->setTaskDependency(0, TaskDependency::MustCompleteBefore, "task_inexistente");

    assert(controller->lastError() != errorBefore);
    assert(controller->getTaskDependencyType(0) == TaskDependency::None);

    std::cout << "  OK" << std::endl;
}

int main() {
    std::cout << "=== TaskController Dependency Tests ===" << std::endl;

    testSetAndGetDependency();
    testRemoveDependency();
    testSelfDependencyRejected();
    testNonexistentDependencyRejected();

    std::cout << "\n=== All TaskController dependency tests passed! ===" << std::endl;
    return 0;
}