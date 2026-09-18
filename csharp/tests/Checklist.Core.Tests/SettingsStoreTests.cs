using Checklist.Persistence;
using Xunit;

namespace Checklist.Core.Tests;

public class SettingsStoreTests
{
    [Fact]
    public void LastCategoryRoundTrips()
    {
        using var dir = new TempDir();
        var store = new SettingsStore(dir.FilePath("settings.json"));
        Assert.Equal("", store.LastCategory);

        store.LastCategory = "Proyecto A";
        Assert.Equal("Proyecto A", store.LastCategory);
    }

    [Fact]
    public void LastCategorySurvivesReload()
    {
        using var dir = new TempDir();
        var path = dir.FilePath("settings.json");

        var first = new SettingsStore(path);
        first.LastCategory = "Cocina";

        var second = new SettingsStore(path);
        Assert.Equal("Cocina", second.LastCategory);
    }

    [Fact]
    public void EmptyFileYieldsEmptyCategory()
    {
        using var dir = new TempDir();
        var store = new SettingsStore(dir.FilePath("settings.json"));
        Assert.Equal("", store.LastCategory);
    }
}