#include "domain/TaskTemplate.h"
#include "controllers/TaskController.h"
#include <iostream>
#include <cassert>
#include <memory>

using namespace checklist;

void testTemplateCreation() {
    std::cout << "Test: Template creation..." << std::endl;
    
    std::vector<TaskTemplateItem> items = {
        {"Buy milk", false},
        {"Buy bread", false},
        {"Buy eggs", false}
    };
    
    auto template_ = std::make_shared<TaskTemplate>("Grocery shopping", items);
    
    assert(template_->name() == "Grocery shopping");
    assert(template_->itemCount() == 3); // items().size()
    assert(template_->items()[0].text == "Buy milk");
    assert(template_->items()[1].text == "Buy bread");
    assert(template_->items()[2].text == "Buy eggs");
    assert(template_->recurrence() == TemplateRecurrence::None);
    
    std::cout << "  OK" << std::endl;
}

void testTemplateRecurrence() {
    std::cout << "Test: Template recurrence..." << std::endl;
    auto template_ = std::make_shared<TaskTemplate>("Weekly report", std::vector<TaskTemplateItem>{});
    template_->setRecurrence(TemplateRecurrence::Weekly, 1);
    
    assert(template_->recurrence() == TemplateRecurrence::Weekly);
    assert(template_->recurrenceInterval() == 1);
    
    template_->setRecurrence(TemplateRecurrence::Monthly, 2);
    assert(template_->recurrence() == TemplateRecurrence::Monthly);
    assert(template_->recurrenceInterval() == 2);
    
    std::cout << "  OK" << std::endl;
}

void testTemplateInstantiate() {
    std::cout << "Test: Template instantiate..." << std::endl;
    std::vector<TaskTemplateItem> items = {
        {"Task 1", false},
        {"Task 2", true}
    };
    
    auto template_ = std::make_shared<TaskTemplate>("My template", items);
    auto task = template_->instantiate("Created task");
    
    assert(task->name() == "Created task");
    assert(task->itemCount() == 2);
    assert(task->getItem(0)->text() == "Task 1");
    assert(!task->getItem(0)->done());
    assert(task->getItem(1)->text() == "Task 2");
    assert(task->getItem(1)->done());
    
    std::cout << "  OK" << std::endl;
}

void testControllerTemplates() {
    std::cout << "Test: Controller templates..." << std::endl;
    auto controller = std::make_shared<TaskController>();
    
    assert(controller->templateCount() == 0);
    
    // Agregar plantilla
    std::vector<TaskTemplateItem> items = {{"Item 1", false}};
    auto template_ = std::make_shared<TaskTemplate>("Test template", items);
    controller->addTemplate(template_);
    
    assert(controller->templateCount() == 1);
    assert(controller->getTemplate(template_->id()) == template_);
    assert(controller->getTemplateByIndex(0) == template_);
    
    // Crear tarea desde plantilla
    auto task = controller->createTaskFromTemplate(template_->id(), "New task from template");
    assert(task != nullptr);
    assert(task->name() == "New task from template");
    assert(task->itemCount() == 1);
    assert(controller->taskCount() == 1);
    
    // Obtener todos los IDs
    auto ids = controller->getAllTemplateIds();
    assert(ids.size() == 1);
    assert(ids[0] == template_->id());
    
    // Remover plantilla
    controller->removeTemplate(template_->id());
    assert(controller->templateCount() == 0);
    assert(controller->getTemplate(template_->id()) == nullptr);
    
    std::cout << "  OK" << std::endl;
}

void testTemplateByRecurrence() {
    std::cout << "Test: Templates by recurrence..." << std::endl;
    auto controller = std::make_shared<TaskController>();
    
    auto daily = std::make_shared<TaskTemplate>("Daily", std::vector<TaskTemplateItem>{{"Check email", false}});
    daily->setRecurrence(TemplateRecurrence::Daily, 1);
    
    auto weekly = std::make_shared<TaskTemplate>("Weekly", std::vector<TaskTemplateItem>{{"Review", false}});
    weekly->setRecurrence(TemplateRecurrence::Weekly, 1);
    
    auto none = std::make_shared<TaskTemplate>("Simple", std::vector<TaskTemplateItem>{{"One time", false}});
    
    controller->addTemplate(daily);
    controller->addTemplate(weekly);
    controller->addTemplate(none);
    
    auto dailyTemplates = controller->getTemplatesByRecurrence(TemplateRecurrence::Daily);
    assert(dailyTemplates.size() == 1);
    
    auto weeklyTemplates = controller->getTemplatesByRecurrence(TemplateRecurrence::Weekly);
    assert(weeklyTemplates.size() == 1);
    
    auto noneTemplates = controller->getTemplatesByRecurrence(TemplateRecurrence::None);
    assert(noneTemplates.size() == 1);
    
    std::cout << "  OK" << std::endl;
}

int main() {
    std::cout << "=== TaskTemplate Tests ===" << std::endl;
    
    testTemplateCreation();
    testTemplateRecurrence();
    testTemplateInstantiate();
    testControllerTemplates();
    testTemplateByRecurrence();
    
    std::cout << "\n=== All template tests passed! ===" << std::endl;
    return 0;
}