using System;
using System.IO;
using System.Text.Json.Nodes;

namespace Checklist.Persistence;

/// <summary>
/// Ajustes simples de la app guardados en disco (por ahora el último tema
/// usado), en la misma carpeta de datos que las tareas.
/// </summary>
public sealed class SettingsStore
{
    private const string FileName = "settings.json";

    private readonly string _filePath;

    public SettingsStore()
    {
        var dataDir = Environment.GetFolderPath(Environment.SpecialFolder.ApplicationData);
        _filePath = Path.Combine(dataDir, "ChecklistApp", "Checklist", FileName);
    }

    public SettingsStore(string filePath)
    {
        _filePath = filePath;
    }

    public string FilePath => _filePath;

    /// <summary>Último tema usado ("" si todavía no hay ninguno).</summary>
    public string LastCategory
    {
        get => Read()?.TryGetPropertyValue("lastCategory", out var value) == true ? (value?.GetValue<string>() ?? "") : "";
        set => Save(value ?? "");
    }

    private JsonObject? Read()
    {
        if (!File.Exists(_filePath)) return null;

        try
        {
            return JsonNode.Parse(File.ReadAllText(_filePath)) as JsonObject;
        }
        catch
        {
            return null;
        }
    }

    private void Save(string lastCategory)
    {
        try
        {
            var dir = Path.GetDirectoryName(_filePath);
            if (dir is not null) Directory.CreateDirectory(dir);

            var json = new JsonObject { ["lastCategory"] = lastCategory };
            File.WriteAllText(_filePath, json.ToJsonString());
        }
        catch
        {
            // El estado de la app no debe romperse si no se puede persistir.
        }
    }
}