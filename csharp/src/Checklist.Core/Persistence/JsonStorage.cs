using System.Text.Json;
using System.Text.Json.Nodes;
using Checklist.Domain;

namespace Checklist.Persistence;

public class JsonStorage
{
    public const int SchemaVersion = 2;

    private string _filePath;

    public JsonStorage()
    {
        var dataDir = Environment.GetFolderPath(Environment.SpecialFolder.ApplicationData);
        _filePath = Path.Combine(dataDir, "ChecklistApp", "Checklist", "tasks.json");
    }

    public JsonStorage(string filePath)
    {
        _filePath = filePath;
    }

    public string FilePath
    {
        get => _filePath;
        set
        {
            if (_filePath != value)
            {
                _filePath = value;
                FilePathChanged?.Invoke();
            }
        }
    }

    public event Action? FilePathChanged;
    public event Action? Saved;
    public event Action? Loaded;
    public event Action<string>? Error;

    // --- Core operations ---

    public List<Task> Load()
    {
        var tasks = new List<Task>();

        if (!File.Exists(_filePath))
        {
            Loaded?.Invoke();
            return tasks;
        }

        string data;
        try
        {
            data = File.ReadAllText(_filePath);
        }
        catch (Exception ex)
        {
            Error?.Invoke("Could not open the file: " + ex.Message);
            return tasks;
        }

        var elements = ParseDocument(data, out var error);
        if (elements is null)
        {
            Error?.Invoke("Error parsing JSON: " + error);
            return tasks;
        }

        foreach (var element in elements)
        {
            var task = JsonToTask(element);
            if (task is not null) tasks.Add(task);
        }

        Loaded?.Invoke();
        return tasks;
    }

    public bool Save(List<Task> tasks)
    {
        if (!EnsureDirectoryExists())
        {
            Error?.Invoke("Could not create the data directory");
            return false;
        }

        var tasksArray = new JsonArray();
        foreach (var task in tasks)
        {
            tasksArray.Add(TaskToJson(task));
        }

        var root = new JsonObject
        {
            ["version"] = SchemaVersion,
            ["tasks"] = tasksArray
        };

        var options = new JsonSerializerOptions { WriteIndented = true };
        var data = root.ToJsonString(options);

        if (WriteAtomic(data))
        {
            Saved?.Invoke();
            return true;
        }

        Error?.Invoke("Could not write the file");
        return false;
    }

    // --- Parsing helper (accepts both new {version, tasks} object and legacy bare array) ---

    private static List<JsonElement>? ParseDocument(string jsonString, out string? error)
    {
        error = null;
        JsonDocument doc;
        try
        {
            doc = JsonDocument.Parse(jsonString);
        }
        catch (JsonException ex)
        {
            error = ex.Message;
            return null;
        }

        using (doc)
        {
            var root = doc.RootElement;
            if (root.ValueKind == JsonValueKind.Object)
            {
                if (root.TryGetProperty("version", out var versionElement)
                    && versionElement.TryGetInt32(out var version)
                    && version > SchemaVersion)
                {
                    error = "Unsupported schema version: " + version;
                    return null;
                }

                if (root.TryGetProperty("tasks", out var tasksElement)
                    && tasksElement.ValueKind == JsonValueKind.Array)
                {
                    return CollectObjectElements(tasksElement);
                }

                return new List<JsonElement>();
            }

            if (root.ValueKind == JsonValueKind.Array)
            {
                return CollectObjectElements(root);
            }

            error = "The JSON file does not contain an object or array";
            return null;
        }
    }

    private static List<JsonElement> CollectObjectElements(JsonElement array)
    {
        var elements = new List<JsonElement>();
        foreach (var value in array.EnumerateArray())
        {
            if (value.ValueKind == JsonValueKind.Object) elements.Add(value.Clone());
        }
        return elements;
    }

    /// <summary>
    /// Parses a raw JSON string into the list of task objects for import.
    /// Returns null when the JSON is malformed (error set), otherwise the list of objects to import.
    /// </summary>
    public static List<JsonElement>? TryParseIntoArray(string jsonString, out string? error)
    {
        error = null;
        try
        {
            return ParseDocument(jsonString, out error);
        }
        catch (JsonException ex)
        {
            error = ex.Message;
            return null;
        }
    }

    // --- Serialization ---

    public static JsonObject ItemToJson(ChecklistItem? item)
    {
        var json = new JsonObject();
        if (item is null) return json;

        json["id"] = item.Id;
        json["text"] = item.Text;
        json["done"] = item.Done;
        return json;
    }

    public static JsonObject CorrectionToJson(Correction? correction)
    {
        var json = new JsonObject();
        if (correction is null) return json;

        var itemsArray = new JsonArray();
        for (var i = 0; i < correction.ItemCount; i++)
        {
            itemsArray.Add(ItemToJson(correction.GetItem(i)));
        }

        json["id"] = correction.Id;
        json["name"] = correction.Name;
        json["date"] = DateToString(correction.Date);
        json["items"] = itemsArray;
        return json;
    }

    public static JsonObject TaskToJson(Task? task)
    {
        var json = new JsonObject();
        if (task is null) return json;

        var itemsArray = new JsonArray();
        for (var i = 0; i < task.ItemCount; i++)
        {
            itemsArray.Add(ItemToJson(task.GetItem(i)));
        }

        var correctionsArray = new JsonArray();
        for (var i = 0; i < task.CorrectionCount; i++)
        {
            correctionsArray.Add(CorrectionToJson(task.GetCorrection(i)));
        }

        json["id"] = task.Id;
        json["name"] = task.Name;
        json["category"] = task.Category ?? "";
        json["date"] = DateToString(task.Date);
        json["items"] = itemsArray;
        json["corrections"] = correctionsArray;
        return json;
    }

    // --- Deserialization ---

    public static ChecklistItem? JsonToItem(JsonElement json)
    {
        var id = json.TryGetProperty("id", out var idEl) ? idEl.GetString() : null;
        var text = json.TryGetProperty("text", out var textEl) ? textEl.GetString() ?? "" : "";
        var done = json.TryGetProperty("done", out var doneEl) && doneEl.ValueKind == JsonValueKind.True;

        if (string.IsNullOrEmpty(id)) id = Guid.NewGuid().ToString("N");

        return new ChecklistItem(id!, text, done);
    }

    public static Correction? JsonToCorrection(JsonElement json)
    {
        var id = json.TryGetProperty("id", out var idEl) ? idEl.GetString() : null;
        var name = json.TryGetProperty("name", out var nameEl) ? nameEl.GetString() : null;
        var date = json.TryGetProperty("date", out var dateEl) ? dateEl.GetString() : null;

        if (string.IsNullOrEmpty(id)) id = Guid.NewGuid().ToString("N");

        var correction = new Correction(id!, name ?? "Unnamed", StringToDate(date));

        if (json.TryGetProperty("items", out var itemsEl) && itemsEl.ValueKind == JsonValueKind.Array)
        {
            foreach (var val in itemsEl.EnumerateArray())
            {
                if (val.ValueKind == JsonValueKind.Object)
                {
                    var item = JsonToItem(val);
                    if (item is not null) correction.AddItem(item);
                }
            }
        }

        return correction;
    }

    public static Task? JsonToTask(JsonElement json)
    {
        var id = json.TryGetProperty("id", out var idEl) ? idEl.GetString() : null;
        var name = json.TryGetProperty("name", out var nameEl) ? nameEl.GetString() : null;
        var date = json.TryGetProperty("date", out var dateEl) ? dateEl.GetString() : null;

        if (string.IsNullOrEmpty(id)) id = Guid.NewGuid().ToString("N");

        var task = new Task(id!, name ?? "Unnamed", StringToDate(date))
        {
            Category = json.TryGetProperty("category", out var categoryEl)
                ? categoryEl.GetString() ?? ""
                : ""
        };

        if (json.TryGetProperty("items", out var itemsEl) && itemsEl.ValueKind == JsonValueKind.Array)
        {
            foreach (var val in itemsEl.EnumerateArray())
            {
                if (val.ValueKind == JsonValueKind.Object)
                {
                    var item = JsonToItem(val);
                    if (item is not null) task.AddItem(item);
                }
            }
        }

        if (json.TryGetProperty("corrections", out var correctionsEl)
            && correctionsEl.ValueKind == JsonValueKind.Array)
        {
            foreach (var val in correctionsEl.EnumerateArray())
            {
                if (val.ValueKind == JsonValueKind.Object)
                {
                    var correction = JsonToCorrection(val);
                    if (correction is not null) task.AddCorrection(correction);
                }
            }
        }

        return task;
    }

    // --- Date helpers (matches web format: yyyy-MM-dd) ---

    public static string DateToString(DateOnly date) => date.ToString("yyyy-MM-dd");

    public static DateOnly StringToDate(string? str)
    {
        if (DateOnly.TryParseExact(str, "yyyy-MM-dd", out var date)) return date;
        return DateOnly.FromDateTime(DateTime.Today);
    }

    // --- Private helpers ---

    private bool EnsureDirectoryExists()
    {
        var directory = Path.GetDirectoryName(_filePath);
        if (string.IsNullOrEmpty(directory)) return false;
        if (Directory.Exists(directory)) return true;
        try
        {
            Directory.CreateDirectory(directory);
            return true;
        }
        catch
        {
            return false;
        }
    }

    private bool WriteAtomic(string data)
    {
        var tempPath = _filePath + ".tmp." + Guid.NewGuid().ToString("N");

        try
        {
            File.WriteAllText(tempPath, data);

            if (File.Exists(_filePath)) File.Delete(_filePath);
            File.Move(tempPath, _filePath);
            return true;
        }
        catch
        {
            try { if (File.Exists(tempPath)) File.Delete(tempPath); }
            catch { /* best effort cleanup */ }
            return false;
        }
    }
}