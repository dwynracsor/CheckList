using Checklist.Domain;
using Checklist.Persistence;
using Xunit;

namespace Checklist.Core.Tests;

public class JsonStorageTests
{
    [Fact]
    public void DateFormatting()
    {
        var date = new DateOnly(2024, 3, 15);
        var str = JsonStorage.DateToString(date);
        Assert.Equal("2024-03-15", str);

        var parsed = JsonStorage.StringToDate(str);
        Assert.Equal(date, parsed);

        var invalid = JsonStorage.StringToDate("invalid");
        Assert.Equal(DateOnly.FromDateTime(DateTime.Today), invalid);
    }

    [Fact]
    public void ItemToJson()
    {
        var item = new ChecklistItem("abc-123", "Test item", true);
        var json = JsonStorage.ItemToJson(item);

        Assert.Equal("abc-123", json["id"]?.ToString());
        Assert.Equal("Test item", json["text"]?.ToString());
        Assert.Equal(true, json["done"]?.GetValue<bool>());
    }

    [Fact]
    public void ItemFromJson()
    {
        var json = new JsonObject { ["id"] = "test-id", ["text"] = "From JSON", ["done"] = true };

        var item = JsonStorage.JsonToItem(json.ToElement());
        Assert.NotNull(item);
        Assert.Equal("test-id", item!.Id);
        Assert.Equal("From JSON", item.Text);
        Assert.True(item.Done);

        var jsonNoId = new JsonObject { ["text"] = "No ID" };
        var item2 = JsonStorage.JsonToItem(jsonNoId.ToElement());
        Assert.NotNull(item2);
        Assert.False(string.IsNullOrEmpty(item2!.Id));
    }

    [Fact]
    public void CorrectionToJson()
    {
        var corr = new Correction("test-id", "Mi Corrección", new DateOnly(2024, 1, 1));
        corr.AddNewItem("Item 1");
        corr.AddNewItem("Item 2");

        var json = JsonStorage.CorrectionToJson(corr);

        Assert.Equal("test-id", json["id"]?.ToString());
        Assert.Equal("Mi Corrección", json["name"]?.ToString());
        Assert.Equal("2024-01-01", json["date"]?.ToString());
        Assert.Equal(2, json["items"]?.AsArray().Count);
    }

    [Fact]
    public void CorrectionFromJson()
    {
        var json = new JsonObject
        {
            ["id"] = "corr-id",
            ["name"] = "Test Corr",
            ["date"] = "2024-05-20",
            ["items"] = new JsonArray(new JsonObject { ["text"] = "Item A", ["done"] = false })
        };

        var corr = JsonStorage.JsonToCorrection(json.ToElement());
        Assert.NotNull(corr);
        Assert.Equal("corr-id", corr!.Id);
        Assert.Equal("Test Corr", corr.Name);
        Assert.Equal(1, corr.ItemCount);
        Assert.Equal("Item A", corr.GetItem(0)?.Text);
    }

    [Fact]
    public void TaskToJson()
    {
        var task = new Task("task-id", "Mi Tarea", new DateOnly(2024, 3, 10));
        task.AddNewItem("Check 1");
        task.AddNewItem("Check 2");
        task.AddNewCorrection("Corrección 1");

        var json = JsonStorage.TaskToJson(task);

        Assert.Equal("task-id", json["id"]?.ToString());
        Assert.Equal("Mi Tarea", json["name"]?.ToString());
        Assert.Equal("2024-03-10", json["date"]?.ToString());
        Assert.Equal(2, json["items"]?.AsArray().Count);
        Assert.Equal(1, json["corrections"]?.AsArray().Count);
    }

    [Fact]
    public void TaskFromJson()
    {
        var json = new JsonObject
        {
            ["id"] = "task-123",
            ["name"] = "From JSON",
            ["date"] = "2024-07-01",
            ["items"] = new JsonArray(new JsonObject { ["text"] = "Check", ["done"] = true }),
            ["corrections"] = new JsonArray(new JsonObject { ["name"] = "Corr", ["date"] = "2024-07-02" })
        };

        var task = JsonStorage.JsonToTask(json.ToElement());
        Assert.NotNull(task);
        Assert.Equal("task-123", task!.Id);
        Assert.Equal("From JSON", task.Name);
        Assert.Equal(1, task.ItemCount);
        Assert.Equal(1, task.CorrectionCount);
        Assert.Equal("Check", task.GetItem(0)?.Text);
        Assert.Equal("Corr", task.GetCorrection(0)?.Name);
    }

    [Fact]
    public void SaveAndLoad()
    {
        using var dir = new TempDir();
        var filePath = dir.FilePath("tasks.json");

        var storage = new JsonStorage(filePath);

        var tasks = new List<Task>
        {
            new("Tarea 1", new DateOnly(2024, 1, 1)),
            new("Tarea 2", new DateOnly(2024, 2, 2))
        };
        tasks[0].AddNewItem("Item 1");
        tasks[0].AddNewItem("Item 2");
        tasks[1].AddNewCorrection("Corrección A");

        Assert.True(storage.Save(tasks));

        var loaded = storage.Load();
        Assert.Equal(2, loaded.Count);
        Assert.Equal("Tarea 1", loaded[0].Name);
        Assert.Equal("2024-01-01", loaded[0].Date.ToString("yyyy-MM-dd"));
        Assert.Equal(2, loaded[0].ItemCount);
        Assert.Equal("Item 1", loaded[0].GetItem(0)?.Text);
        Assert.Equal("Item 2", loaded[0].GetItem(1)?.Text);
        Assert.Equal("Tarea 2", loaded[1].Name);
        Assert.Equal(1, loaded[1].CorrectionCount);
        Assert.Equal("Corrección A", loaded[1].GetCorrection(0)?.Name);
    }

    [Fact]
    public void CategoryRoundTrip()
    {
        using var dir = new TempDir();
        var filePath = dir.FilePath("tasks.json");
        var storage = new JsonStorage(filePath);

        var task = new Task("Tarea", new DateOnly(2024, 3, 10)) { Category = "Proyecto X" };

        Assert.True(storage.Save(new List<Task> { task }));
        var loaded = storage.Load();

        Assert.Single(loaded);
        Assert.Equal("Proyecto X", loaded[0].Category);
    }

    [Fact]
    public void LegacyTaskWithoutCategoryLoadsAsEmpty()
    {
        var json = new JsonObject
        {
            ["id"] = "legacy-1",
            ["name"] = "Legacy",
            ["date"] = "2024-01-01"
        };

        var task = JsonStorage.JsonToTask(json.ToElement());
        Assert.NotNull(task);
        Assert.Equal("Legacy", task!.Name);
        Assert.Equal("", task.Category);
    }

    [Fact]
    public void SaveWritesSchemaVersionTwo()
    {
        using var dir = new TempDir();
        var filePath = dir.FilePath("tasks.json");
        var storage = new JsonStorage(filePath);

        Assert.True(storage.Save(new List<Task> { new("T", new DateOnly(2024, 1, 1)) }));

        var raw = File.ReadAllText(filePath);
        Assert.Contains("\"version\": 2", raw);
        Assert.Contains("\"category\"", raw);
    }

    [Fact]
    public void LoadNonExistentFile()
    {
        var storage = new JsonStorage(Path.Combine(Path.GetTempPath(), "nonexistent-" + Guid.NewGuid(), "tasks.json"));
        Assert.Empty(storage.Load());
    }

    [Fact]
    public void LoadCorruptedJson()
    {
        using var dir = new TempDir();
        var filePath = dir.FilePath("corrupted.json");
        File.WriteAllText(filePath, "{ this is not valid json }}}");

        var storage = new JsonStorage(filePath);
        Assert.Empty(storage.Load());
    }

    [Fact]
    public void LoadEmptyArray()
    {
        using var dir = new TempDir();
        var filePath = dir.FilePath("empty.json");
        File.WriteAllText(filePath, "[]");

        var storage = new JsonStorage(filePath);
        Assert.Empty(storage.Load());
    }

    [Fact]
    public void LoadMissingFields()
    {
        using var dir = new TempDir();
        var filePath = dir.FilePath("minimal.json");
        File.WriteAllText(filePath, """[{"name": "Minimal Task"}]""");

        var storage = new JsonStorage(filePath);
        var tasks = storage.Load();
        Assert.Single(tasks);
        Assert.Equal("Minimal Task", tasks[0].Name);
        Assert.Equal(0, tasks[0].ItemCount);
        Assert.Equal(0, tasks[0].CorrectionCount);
    }
}