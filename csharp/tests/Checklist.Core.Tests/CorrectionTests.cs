using Checklist.Domain;
using Xunit;

namespace Checklist.Core.Tests;

public class CorrectionTests
{
    [Fact]
    public void Defaults()
    {
        var corr = new Correction();
        Assert.False(string.IsNullOrEmpty(corr.Id));
        Assert.Equal("Unnamed", corr.Name);
        Assert.False(string.IsNullOrEmpty(corr.Date.ToString("yyyy-MM-dd")));
        Assert.Equal(0, corr.ItemCount);
    }

    [Fact]
    public void NameProperty()
    {
        var corr = new Correction("Mi Corrección");
        Assert.Equal("Mi Corrección", corr.Name);

        corr.Name = "Actualizada";
        Assert.Equal("Actualizada", corr.Name);
    }

    [Fact]
    public void DateProperty()
    {
        var today = new DateOnly(2024, 1, 15);
        var corr = new Correction("Test", today);
        Assert.Equal("2024-01-15", corr.Date.ToString("yyyy-MM-dd"));
    }

    [Fact]
    public void AddItem()
    {
        var corr = new Correction();
        var item = new ChecklistItem("Item 1");
        corr.AddItem(item);

        Assert.Equal(1, corr.ItemCount);
        Assert.Equal("Item 1", corr.GetItem(0)?.Text);
    }

    [Fact]
    public void RemoveItem()
    {
        var corr = new Correction();
        corr.AddNewItem("Item 1");
        corr.AddNewItem("Item 2");
        Assert.Equal(2, corr.ItemCount);

        corr.RemoveItem(0);
        Assert.Equal(1, corr.ItemCount);
        Assert.Equal("Item 2", corr.GetItem(0)?.Text);
    }

    [Fact]
    public void GetItem()
    {
        var corr = new Correction();
        corr.AddNewItem("Item 1");

        var item = corr.GetItem(0);
        Assert.NotNull(item);
        Assert.Equal("Item 1", item!.Text);

        Assert.Null(corr.GetItem(-1));
        Assert.Null(corr.GetItem(1));
    }

    [Fact]
    public void ItemCount()
    {
        var corr = new Correction();
        Assert.Equal(0, corr.ItemCount);

        corr.AddNewItem("A");
        Assert.Equal(1, corr.ItemCount);

        corr.AddNewItem("B");
        Assert.Equal(2, corr.ItemCount);
    }

    [Fact]
    public void AddNewItem()
    {
        var corr = new Correction();
        corr.AddNewItem("New item");

        Assert.Equal(1, corr.ItemCount);
        Assert.Equal("New item", corr.GetItem(0)?.Text);
        Assert.False(corr.GetItem(0)?.Done);
    }

    [Fact]
    public void Signals()
    {
        var corr = new Correction();
        var nameCount = 0;
        var countCount = 0;
        corr.NameChanged += () => nameCount++;
        corr.ItemCountChanged += () => countCount++;

        corr.Name = "New name";
        Assert.Equal(1, nameCount);

        corr.AddNewItem("Item");
        Assert.Equal(1, countCount);
    }
}