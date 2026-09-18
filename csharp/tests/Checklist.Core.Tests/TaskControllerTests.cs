using Checklist.Controllers;
using Checklist.Domain;
using Xunit;

namespace Checklist.Core.Tests;

public class TaskControllerTests
{
    [Fact]
    public void Defaults()
    {
        using var controller = new TaskController();
        Assert.Equal(0, controller.TaskCount);
        Assert.False(controller.IsLoaded);
        Assert.Empty(controller.LastError);
        Assert.True(controller.AutoSave);
        Assert.Equal(500, controller.AutoSaveDelay);
    }

    [Fact]
    public void AddTask()
    {
        using var controller = new TaskController();
        controller.AutoSave = false;
        Task? task = controller.AddTask("Mi Tarea");

        Assert.NotNull(task);
        Assert.Equal(1, controller.TaskCount);
        Assert.Equal("Mi Tarea", task!.Name);
    }

    [Fact]
    public void AddTaskEmptyName()
    {
        using var controller = new TaskController();
        controller.AutoSave = false;
        Task? task = controller.AddTask("");

        Assert.Null(task);
        Assert.Equal(0, controller.TaskCount);
        Assert.NotEmpty(controller.LastError);
    }

    [Fact]
    public void RemoveTask()
    {
        using var controller = new TaskController();
        controller.AutoSave = false;
        controller.AddTask("Tarea 1");
        controller.AddTask("Tarea 2");

        controller.RemoveTask(0);
        Assert.Equal(1, controller.TaskCount);
        Assert.Equal("Tarea 2", controller.GetTask(0)?.Name);
    }

    [Fact]
    public void GetTaskInvalidIndex()
    {
        using var controller = new TaskController();
        Assert.Null(controller.GetTask(-1));
        Assert.Null(controller.GetTask(0));
        Assert.Null(controller.GetTask(100));
    }

    [Fact]
    public void AddItemToTask()
    {
        using var controller = new TaskController();
        controller.AutoSave = false;
        controller.AddTask("Tarea");

        controller.AddItemToTask(0, "Item 1");
        controller.AddItemToTask(0, "Item 2");

        var task = controller.GetTask(0);
        Assert.Equal(2, task?.ItemCount);
        Assert.Equal("Item 1", task?.GetItem(0)?.Text);
    }

    [Fact]
    public void RemoveItemFromTask()
    {
        using var controller = new TaskController();
        controller.AutoSave = false;
        controller.AddTask("Tarea");
        controller.AddItemToTask(0, "Item 1");
        controller.AddItemToTask(0, "Item 2");

        controller.RemoveItemFromTask(0, 0);
        Assert.Equal(1, controller.GetTask(0)?.ItemCount);
        Assert.Equal("Item 2", controller.GetTask(0)?.GetItem(0)?.Text);
    }

    [Fact]
    public void ToggleItemInTask()
    {
        using var controller = new TaskController();
        controller.AutoSave = false;
        controller.AddTask("Tarea");
        controller.AddItemToTask(0, "Item 1");

        controller.ToggleItemInTask(0, 0, true);
        Assert.True(controller.GetTask(0)?.GetItem(0)?.Done);

        controller.ToggleItemInTask(0, 0, false);
        Assert.False(controller.GetTask(0)?.GetItem(0)?.Done);
    }

    [Fact]
    public void AddCorrectionToTask()
    {
        using var controller = new TaskController();
        controller.AutoSave = false;
        controller.AddTask("Tarea");

        var corr = controller.AddCorrectionToTask(0, "Corrección 1");
        Assert.NotNull(corr);
        Assert.Equal(1, controller.GetTask(0)?.CorrectionCount);
        Assert.Equal("Corrección 1", corr?.Name);
    }

    [Fact]
    public void RemoveCorrectionFromTask()
    {
        using var controller = new TaskController();
        controller.AutoSave = false;
        controller.AddTask("Tarea");
        controller.AddCorrectionToTask(0, "Corr 1");
        controller.AddCorrectionToTask(0, "Corr 2");

        controller.RemoveCorrectionFromTask(0, 0);
        Assert.Equal(1, controller.GetTask(0)?.CorrectionCount);
        Assert.Equal("Corr 2", controller.GetTask(0)?.GetCorrection(0)?.Name);
    }

    [Fact]
    public void AddItemToCorrection()
    {
        using var controller = new TaskController();
        controller.AutoSave = false;
        controller.AddTask("Tarea");
        controller.AddCorrectionToTask(0, "Corr 1");

        controller.AddItemToCorrection(0, 0, "Item 1");
        Assert.Equal(1, controller.GetTask(0)?.GetCorrection(0)?.ItemCount);
        Assert.Equal("Item 1", controller.GetTask(0)?.GetCorrection(0)?.GetItem(0)?.Text);
    }

    [Fact]
    public void RemoveItemFromCorrection()
    {
        using var controller = new TaskController();
        controller.AutoSave = false;
        controller.AddTask("Tarea");
        controller.AddCorrectionToTask(0, "Corr 1");
        controller.AddItemToCorrection(0, 0, "Item 1");
        controller.AddItemToCorrection(0, 0, "Item 2");

        controller.RemoveItemFromCorrection(0, 0, 0);
        Assert.Equal(1, controller.GetTask(0)?.GetCorrection(0)?.ItemCount);
        Assert.Equal("Item 2", controller.GetTask(0)?.GetCorrection(0)?.GetItem(0)?.Text);
    }

    [Fact]
    public void SetItemTextInCorrection()
    {
        using var controller = new TaskController();
        controller.AutoSave = false;
        controller.AddTask("Tarea");
        controller.AddCorrectionToTask(0, "Corr 1");
        controller.AddItemToCorrection(0, 0, "Item 1");

        controller.SetItemTextInCorrection(0, 0, 0, "  Item nuevo  ");
        Assert.Equal("Item nuevo", controller.GetTask(0)?.GetCorrection(0)?.GetItem(0)?.Text);

        controller.Undo();
        Assert.Equal("Item 1", controller.GetTask(0)?.GetCorrection(0)?.GetItem(0)?.Text);

        controller.Redo();
        Assert.Equal("Item nuevo", controller.GetTask(0)?.GetCorrection(0)?.GetItem(0)?.Text);
    }

    [Fact]
    public void ToggleItemInCorrection()
    {
        using var controller = new TaskController();
        controller.AutoSave = false;
        controller.AddTask("Tarea");
        controller.AddCorrectionToTask(0, "Corr 1");
        controller.AddItemToCorrection(0, 0, "Item 1");

        controller.ToggleItemInCorrection(0, 0, 0, true);
        Assert.True(controller.GetTask(0)?.GetCorrection(0)?.GetItem(0)?.Done);

        controller.ToggleItemInCorrection(0, 0, 0, false);
        Assert.False(controller.GetTask(0)?.GetCorrection(0)?.GetItem(0)?.Done);
    }

    [Fact]
    public void SetTaskName()
    {
        using var controller = new TaskController();
        controller.AutoSave = false;
        controller.AddTask("Original");

        controller.SetTaskName(0, "Actualizada");
        Assert.Equal("Actualizada", controller.GetTask(0)?.Name);
    }

    [Fact]
    public void SetCorrectionName()
    {
        using var controller = new TaskController();
        controller.AutoSave = false;
        controller.AddTask("Tarea");
        controller.AddCorrectionToTask(0, "Original");

        controller.SetCorrectionName(0, 0, "Actualizada");
        Assert.Equal("Actualizada", controller.GetTask(0)?.GetCorrection(0)?.Name);
    }

    [Fact]
    public void SaveAndLoad()
    {
        using var dir = new TempDir();
        var filePath = dir.FilePath("tasks.json");

        // Save
        {
            using var controller = new TaskController(filePath);
            controller.AutoSave = false;
            controller.AddTask("Tarea 1");
            controller.AddItemToTask(0, "Item 1");
            controller.AddCorrectionToTask(0, "Corr 1");
            controller.AddItemToCorrection(0, 0, "Corr Item 1");
            Assert.True(controller.Save());
        }

        // Load
        {
            using var controller = new TaskController(filePath);
            controller.AutoSave = false;
            controller.Load();
            Assert.True(controller.IsLoaded);
            Assert.Equal(1, controller.TaskCount);

            var task = controller.GetTask(0);
            Assert.Equal("Tarea 1", task?.Name);
            Assert.Equal(1, task?.ItemCount);
            Assert.Equal("Item 1", task?.GetItem(0)?.Text);
            Assert.Equal(1, task?.CorrectionCount);
            Assert.Equal("Corr 1", task?.GetCorrection(0)?.Name);
            Assert.Equal(1, task?.GetCorrection(0)?.ItemCount);
            Assert.Equal("Corr Item 1", task?.GetCorrection(0)?.GetItem(0)?.Text);
        }
    }

    [Fact]
    public void LoadNonExistentFile()
    {
        using var controller = new TaskController(Path.Combine(Path.GetTempPath(), "nonexistent-" + Guid.NewGuid(), "tasks.json"));
        controller.AutoSave = false;
        controller.Load();
        Assert.True(controller.IsLoaded);
        Assert.Equal(0, controller.TaskCount);
    }

    [Fact]
    public void ImportFromJsonString()
    {
        using var controller = new TaskController();
        controller.AutoSave = false;

        const string json = """
            [
              {
                "id": "test-1",
                "name": "Imported Task",
                "date": "2024-01-15",
                "items": [
                  { "id": "i1", "text": "Imported Item", "done": false }
                ],
                "corrections": []
              }
            ]
            """;

        controller.ImportFromJsonString(json);
        Assert.Equal(1, controller.TaskCount);
        Assert.Equal("Imported Task", controller.GetTask(0)?.Name);
        Assert.Equal(1, controller.GetTask(0)?.ItemCount);
    }

    [Fact]
    public void ImportInvalidJson()
    {
        using var controller = new TaskController();
        controller.ImportFromJsonString("not valid json");
        Assert.Equal(0, controller.TaskCount);
        Assert.NotEmpty(controller.LastError);
    }

    [Fact]
    public void AutoSaveFlags()
    {
        using var controller = new TaskController();
        Assert.True(controller.AutoSave);

        controller.AutoSave = false;
        Assert.False(controller.AutoSave);

        controller.AutoSaveDelay = 1000;
        Assert.Equal(1000, controller.AutoSaveDelay);
    }

    [Fact]
    public void Signals()
    {
        using var controller = new TaskController();
        controller.AutoSave = false;

        var tasksCount = 0;
        var addedCount = 0;
        var removedCount = 0;
        controller.TasksChanged += () => tasksCount++;
        controller.TaskAdded += _ => addedCount++;
        controller.TaskRemoved += _ => removedCount++;

        controller.AddTask("Tarea");
        Assert.Equal(1, tasksCount);
        Assert.Equal(1, addedCount);

        controller.RemoveTask(0);
        Assert.Equal(2, tasksCount);
        Assert.Equal(1, removedCount);
    }

    [Fact]
    public void UndoRedoTaskLifecycle()
    {
        using var controller = new TaskController();
        controller.AutoSave = false;

        controller.AddTask("Tarea");
        Assert.Equal(1, controller.TaskCount);
        Assert.True(controller.CanUndo);
        Assert.False(controller.CanRedo);

        controller.Undo();
        Assert.Equal(0, controller.TaskCount);

        controller.Redo();
        Assert.Equal(1, controller.TaskCount);
        Assert.Equal("Tarea", controller.GetTask(0)?.Name);
    }

    [Fact]
    public void AddTaskWithCategory()
    {
        using var controller = new TaskController();
        controller.AutoSave = false;

        var task = controller.AddTask("Tarea", "Proyecto A");
        Assert.NotNull(task);
        Assert.Equal("Proyecto A", task!.Category);
        Assert.Equal("Proyecto A", controller.GetTask(0)?.Category);
    }

    [Fact]
    public void AddTaskCategoryTrimmed()
    {
        using var controller = new TaskController();
        controller.AutoSave = false;

        var task = controller.AddTask("Tarea", "  Proyecto A  ");
        Assert.Equal("Proyecto A", task?.Category);
    }

    [Fact]
    public void SetTaskCategory()
    {
        using var controller = new TaskController();
        controller.AutoSave = false;
        controller.AddTask("Tarea");

        controller.SetTaskCategory(0, "Nuevo Tema");
        Assert.Equal("Nuevo Tema", controller.GetTask(0)?.Category);

        controller.SetTaskCategory(0, "");
        Assert.Equal("", controller.GetTask(0)?.Category);
    }

    [Fact]
    public void SetTaskCategoryUndoRedo()
    {
        using var controller = new TaskController();
        controller.AutoSave = false;
        controller.AddTask("Tarea");

        controller.SetTaskCategory(0, "Tema B");
        Assert.True(controller.CanUndo);

        controller.Undo();
        Assert.Equal("", controller.GetTask(0)?.Category);

        controller.Redo();
        Assert.Equal("Tema B", controller.GetTask(0)?.Category);
    }
}