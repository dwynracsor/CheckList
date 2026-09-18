using System.Text.Json;
using System.Text.Json.Nodes;

namespace Checklist.Core.Tests;

internal sealed class TempDir : IDisposable
{
    public TempDir()
    {
        Path = System.IO.Path.Combine(System.IO.Path.GetTempPath(), "checklist-tests-" + Guid.NewGuid().ToString("N"));
        Directory.CreateDirectory(Path);
    }

    public string Path { get; }

    public string FilePath(string name) => System.IO.Path.Combine(Path, name);

    public void Dispose()
    {
        try
        {
            if (Directory.Exists(Path)) Directory.Delete(Path, true);
        }
        catch
        {
            // best effort cleanup
        }
    }
}

internal static class JsonTestExtensions
{
    public static JsonElement ToElement(this JsonNode? node)
    {
        return JsonDocument.Parse((node ?? new JsonObject()).ToJsonString()).RootElement;
    }
}