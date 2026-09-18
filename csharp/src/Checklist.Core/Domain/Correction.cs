using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Runtime.CompilerServices;

namespace Checklist.Domain;

public class Correction : INotifyPropertyChanged
{
    private readonly ObservableCollection<ChecklistItem> _items = new();
    private string _name;

    public event PropertyChangedEventHandler? PropertyChanged;

    public Correction() : this(Guid.NewGuid().ToString("N"), "Unnamed", DateOnly.FromDateTime(DateTime.Today))
    {
    }

    public Correction(string name, DateOnly? date = null)
        : this(Guid.NewGuid().ToString("N"), name, date ?? DateOnly.FromDateTime(DateTime.Today))
    {
    }

    public Correction(string id, string name, DateOnly? date)
    {
        Id = string.IsNullOrEmpty(id) ? Guid.NewGuid().ToString("N") : id;
        _name = name;
        Date = date ?? DateOnly.FromDateTime(DateTime.Today);
    }

    public string Id { get; }

    public string Name
    {
        get => _name;
        set
        {
            if (_name != value)
            {
                _name = value;
                OnPropertyChanged();
                NameChanged?.Invoke();
            }
        }
    }

    public DateOnly Date { get; }

    public IReadOnlyList<ChecklistItem> Items => _items;

    public int ItemCount => _items.Count;

    public event Action? NameChanged;
    public event Action? ItemCountChanged;
    public event Action? ItemsChanged;

    public void AddItem(ChecklistItem item)
    {
        if (item is null) return;
        _items.Add(item);
        ItemCountChanged?.Invoke();
        ItemsChanged?.Invoke();
    }

    public void RemoveItem(int index)
    {
        if (index < 0 || index >= _items.Count) return;
        _items.RemoveAt(index);
        ItemCountChanged?.Invoke();
        ItemsChanged?.Invoke();
    }

    public ChecklistItem? GetItem(int index)
        => index < 0 || index >= _items.Count ? null : _items[index];

    public void AddNewItem(string text) => AddItem(new ChecklistItem(text, false));

    private void OnPropertyChanged([CallerMemberName] string? propertyName = null)
        => PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(propertyName));
}