using Checklist.Domain;
using Xunit;

namespace Checklist.Core.Tests;

public class ChecklistItemTests
{
    [Fact]
    public void Defaults()
    {
        var item = new ChecklistItem();
        Assert.False(string.IsNullOrEmpty(item.Id));
        Assert.Equal("", item.Text);
        Assert.False(item.Done);
    }

    [Fact]
    public void TextProperty()
    {
        var item = new ChecklistItem("Test item");
        Assert.Equal("Test item", item.Text);

        item.Text = "Updated";
        Assert.Equal("Updated", item.Text);
    }

    [Fact]
    public void DoneProperty()
    {
        var item = new ChecklistItem("Test");
        Assert.False(item.Done);

        item.Done = true;
        Assert.True(item.Done);
    }

    [Fact]
    public void Toggle()
    {
        var item = new ChecklistItem("Test");
        Assert.False(item.Done);

        item.Toggle();
        Assert.True(item.Done);

        item.Toggle();
        Assert.False(item.Done);
    }

    [Fact]
    public void SignalTextChanged()
    {
        var item = new ChecklistItem("Original");
        var count = 0;
        item.TextChanged += () => count++;

        item.Text = "Changed";
        Assert.Equal(1, count);

        item.Text = "Changed";
        Assert.Equal(1, count);
    }

    [Fact]
    public void SignalDoneChanged()
    {
        var item = new ChecklistItem();
        var count = 0;
        item.DoneChanged += () => count++;

        item.Done = true;
        Assert.Equal(1, count);

        item.Done = true;
        Assert.Equal(1, count);
    }
}