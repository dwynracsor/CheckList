using Checklist.Domain;
using Xunit;

namespace Checklist.Core.Tests;

public class TaskTests
{
    [Fact]
    public void Defaults()
    {
        var task = new Task();
        Assert.False(string.IsNullOrEmpty(task.Id));
        Assert.Equal("Unnamed", task.Name);
        Assert.False(string.IsNullOrEmpty(task.Date.ToString("yyyy-MM-dd")));
        Assert.Equal(0, task.ItemCount);
        Assert.Equal(0, task.CorrectionCount);
    }

    [Fact]
    public void NameProperty()
    {
        var task = new Task("Mi Tarea");
        Assert.Equal("Mi Tarea", task.Name);

        task.Name = "Actualizada";
        Assert.Equal("Actualizada", task.Name);
    }

    [Fact]
    public void DateProperty()
    {
        var today = new DateOnly(2024, 6, 20);
        var task = new Task("Test", today);
        Assert.Equal("2024-06-20", task.Date.ToString("yyyy-MM-dd"));
    }

    [Fact]
    public void AddItem()
    {
        var task = new Task();
        task.AddNewItem("Item 1");
        task.AddNewItem("Item 2");

        Assert.Equal(2, task.ItemCount);
        Assert.Equal("Item 1", task.GetItem(0)?.Text);
        Assert.Equal("Item 2", task.GetItem(1)?.Text);
    }

    [Fact]
    public void RemoveItem()
    {
        var task = new Task();
        task.AddNewItem("Item 1");
        task.AddNewItem("Item 2");

        task.RemoveItem(0);
        Assert.Equal(1, task.ItemCount);
        Assert.Equal("Item 2", task.GetItem(0)?.Text);
    }

    [Fact]
    public void AddCorrection()
    {
        var task = new Task();
        var corr = task.AddNewCorrection("Corrección 1");

        Assert.Equal(1, task.CorrectionCount);
        Assert.NotNull(corr);
        Assert.Equal("Corrección 1", corr.Name);
    }

    [Fact]
    public void RemoveCorrection()
    {
        var task = new Task();
        task.AddNewCorrection("Corr 1");
        task.AddNewCorrection("Corr 2");

        task.RemoveCorrection(0);
        Assert.Equal(1, task.CorrectionCount);
        Assert.Equal("Corr 2", task.GetCorrection(0)?.Name);
    }

    [Fact]
    public void Signals()
    {
        var task = new Task();
        var nameCount = 0;
        var itemCountCount = 0;
        var corrCountCount = 0;
        task.NameChanged += () => nameCount++;
        task.ItemCountChanged += () => itemCountCount++;
        task.CorrectionCountChanged += () => corrCountCount++;

        task.Name = "New name";
        Assert.Equal(1, nameCount);

        task.AddNewItem("Item");
        Assert.Equal(1, itemCountCount);

        task.AddNewCorrection("Corr");
        Assert.Equal(1, corrCountCount);
    }

    [Fact]
    public void CategoryProperty()
    {
        var task = new Task("Tarea");
        Assert.Equal("", task.Category);

        var changedCount = 0;
        task.CategoryChanged += () => changedCount++;

        task.Category = "Proyecto A";
        Assert.Equal("Proyecto A", task.Category);
        Assert.Equal(1, changedCount);

        task.Category = null!;
        Assert.Equal("", task.Category);
        Assert.Equal(2, changedCount);
    }

    [Fact]
    public void IsFinished()
    {
        var task = new Task("Tarea");
        Assert.False(task.IsFinished);
        Assert.False(task.IsFinished);

        task.AddNewItem("Item 1");
        task.AddNewItem("Item 2");
        Assert.False(task.IsFinished);

        task.GetItem(0)!.Done = true;
        Assert.False(task.IsFinished);

        task.GetItem(1)!.Done = true;
        Assert.True(task.IsFinished);
    }
}