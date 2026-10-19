#include "domain/Task.h"
#include <iostream>
#include <cassert>
#include <thread>
#include <chrono>

using namespace checklist;

void testEstimatedDuration() {
    std::cout << "Test: Estimated duration..." << std::endl;
    auto task = std::make_shared<Task>("Test task", Date(2024, 12, 25));
    
    task->setEstimatedDurationSeconds(3600); // 1 hour
    assert(task->estimatedDurationSeconds() == 3600);
    assert(task->remainingTimeSeconds() == 3600); // No time spent yet
    
    task->setEstimatedDurationSeconds(0); // Reset
    assert(task->estimatedDurationSeconds() == 0);
    assert(task->remainingTimeSeconds() == -1); // No estimation
    
    std::cout << "  OK" << std::endl;
}

void testPomodoro() {
    std::cout << "Test: Pomodoro counter..." << std::endl;
    auto task = std::make_shared<Task>("Test task", Date(2024, 12, 25));
    
    assert(task->pomodoroCount() == 0);
    
    task->incrementPomodoro();
    assert(task->pomodoroCount() == 1);
    
    task->incrementPomodoro();
    task->incrementPomodoro();
    assert(task->pomodoroCount() == 3);
    
    task->resetPomodoros();
    assert(task->pomodoroCount() == 0);
    
    std::cout << "  OK" << std::endl;
}

void testTiming() {
    std::cout << "Test: Timing session..." << std::endl;
    auto task = std::make_shared<Task>("Test task", Date(2024, 12, 25));
    
    assert(!task->isTiming());
    assert(task->actualDurationSeconds() == 0);
    
    task->startTiming();
    assert(task->isTiming());
    
    // Wait 1 second
    std::this_thread::sleep_for(std::chrono::seconds(1));
    
    task->stopTiming();
    assert(!task->isTiming());
    assert(task->actualDurationSeconds() >= 1); // At least 1 second
    
    std::cout << "  OK (actual: " << task->actualDurationSeconds() << "s)" << std::endl;
}

void testProgressPercentage() {
    std::cout << "Test: Progress percentage..." << std::endl;
    auto task = std::make_shared<Task>("Test task", Date(2024, 12, 25));
    
    task->setEstimatedDurationSeconds(100); // 100 seconds
    
    assert(task->progressPercentage() == 0); // No time spent
    
    // Simulate time spent
    // We can't easily set actualDurationSeconds directly, but we can check 0
    task->setEstimatedDurationSeconds(0);
    assert(task->progressPercentage() == 0); // No estimation set
    
    std::cout << "  OK" << std::endl;
}

void testCallbacks() {
    std::cout << "Test: Callbacks..." << std::endl;
    auto task = std::make_shared<Task>("Test task", Date(2024, 12, 25));
    
    bool durationChanged = false;
    bool pomodoroChanged = false;
    bool timingChanged = false;
    
    task->setDurationChangedCallback([&]() { durationChanged = true; });
    task->setPomodoroChangedCallback([&]() { pomodoroChanged = true; });
    task->setTimingStateChangedCallback([&]() { timingChanged = true; });
    
    task->setEstimatedDurationSeconds(3600);
    assert(durationChanged);
    
    task->incrementPomodoro();
    assert(pomodoroChanged);
    
    task->startTiming();
    assert(timingChanged);
    
    std::cout << "  OK" << std::endl;
}

void testCopyMove() {
    std::cout << "Test: Copy/move with time data..." << std::endl;
    auto task1 = std::make_shared<Task>("Original", Date(2024, 12, 25));
    task1->setEstimatedDurationSeconds(3600);
    task1->incrementPomodoro();
    task1->incrementPomodoro();
    
    // Copy
    auto task2 = std::make_shared<Task>(*task1);
    assert(task2->estimatedDurationSeconds() == 3600);
    assert(task2->pomodoroCount() == 2);
    
    // Move
    auto task3 = std::move(task2);
    assert(task3->estimatedDurationSeconds() == 3600);
    assert(task3->pomodoroCount() == 2);
    
    std::cout << "  OK" << std::endl;
}

int main() {
    std::cout << "=== Task Time & Pomodoro Tests ===" << std::endl;
    
    testEstimatedDuration();
    testPomodoro();
    testTiming();
    testProgressPercentage();
    testCallbacks();
    testCopyMove();
    
    std::cout << "\n=== All time & pomodoro tests passed! ===" << std::endl;
    return 0;
}