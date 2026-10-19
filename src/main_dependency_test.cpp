#include "domain/Task.h"
#include <iostream>
#include <cassert>
#include <memory>

using namespace checklist;

void testDependencyNone() {
    std::cout << "Test: Sin dependencia..." << std::endl;
    auto task = std::make_shared<Task>("Tarea independiente", Date(2024, 12, 25));
    
    assert(task->dependencyType() == TaskDependency::None);
    assert(task->dependencyId() == "");
    assert(!task->isBlockedByDependency(nullptr));
    assert(task->canComplete());
    
    std::cout << "  OK" << std::endl;
}

void testDependencyMustCompleteBefore() {
    std::cout << "Test: Dependencia MustCompleteBefore..." << std::endl;
    auto parent = std::make_shared<Task>("Tarea padre", Date(2024, 12, 25));
    auto child = std::make_shared<Task>("Tarea hija", Date(2024, 12, 26));
    
    child->setDependency(TaskDependency::MustCompleteBefore, parent->id());
    
    assert(child->dependencyType() == TaskDependency::MustCompleteBefore);
    assert(child->dependencyId() == parent->id());
    
    // Padre no está terminado, hija está bloqueada
    assert(child->isBlockedByDependency(parent));
    assert(!child->canComplete());
    
    // Padre termina -> hija se desbloquea
    // Simular que el padre está terminado (todos los items hechos)
    // No podemos marcar items fácilmente sin el controlador, pero verificamos la lógica
    // de isBlockedByDependency
    
    std::cout << "  OK" << std::endl;
}

void testDependencyNonexistent() {
    std::cout << "Test: Dependencia a tarea inexistente..." << std::endl;
    auto child = std::make_shared<Task>("Tarea huérfana", Date(2024, 12, 26));
    child->setDependency(TaskDependency::MustCompleteBefore, "task_inexistente");
    
    // La tarea dependiente no existe, está bloqueada
    assert(child->isBlockedByDependency(nullptr));
    
    std::cout << "  OK" << std::endl;
}

void testDependencyChangeCallback() {
    std::cout << "Test: Callback de cambio de dependencia..." << std::endl;
    auto task = std::make_shared<Task>("Tarea", Date(2024, 12, 25));
    
    bool callbackCalled = false;
    task->setDependencyChangedCallback([&]() { callbackCalled = true; });
    
    task->setDependency(TaskDependency::MustCompleteBefore, "task_parent");
    assert(callbackCalled);
    
    std::cout << "  OK" << std::endl;
}

void testDependencyCopyMove() {
    std::cout << "Test: Copy/move con dependencias..." << std::endl;
    auto task1 = std::make_shared<Task>("Original", Date(2024, 12, 25));
    task1->setDependency(TaskDependency::MustCompleteBefore, "task_parent");
    
    // Copy
    auto task2 = std::make_shared<Task>(*task1);
    assert(task2->dependencyType() == TaskDependency::MustCompleteBefore);
    assert(task2->dependencyId() == "task_parent");
    
    // Move
    auto task3 = std::move(task2);
    assert(task3->dependencyType() == TaskDependency::MustCompleteBefore);
    assert(task3->dependencyId() == "task_parent");
    
    std::cout << "  OK" << std::endl;
}

int main() {
    std::cout << "=== Task Dependencies Tests ===" << std::endl;
    
    testDependencyNone();
    testDependencyMustCompleteBefore();
    testDependencyNonexistent();
    testDependencyChangeCallback();
    testDependencyCopyMove();
    
    std::cout << "\n=== All dependency tests passed! ===" << std::endl;
    return 0;
}