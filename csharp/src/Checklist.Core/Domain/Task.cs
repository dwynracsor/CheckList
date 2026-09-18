using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Linq;
using System.Runtime.CompilerServices;

namespace Checklist.Domain;

public class Task : INotifyPropertyChanged
{
    private readonly ObservableCollection<ChecklistItem> _items = new();
    private readonly ObservableCollection<Correction> _corrections = new();
    private string _name;
    private string _category = "";

    public event PropertyChangedEventHandler? PropertyChanged;

    public Task() : this(Guid.NewGuid().ToString("N"), "Unnamed", null)
    {
    }

    public Task(string name, DateOnly? date = null)
        : this(Guid.NewGuid().ToString("N"), name, date)
    {
    }

    public Task(string id, string name, DateOnly? date)
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

    /// <summary>Tema / proyecto al que pertenece la lista (vacío = sin tema).</summary>
    public string Category
    {
        get => _category;
        set
        {
            value ??= "";
            if (_category != value)
            {
                _category = value;
                OnPropertyChanged();
                CategoryChanged?.Invoke();
            }
        }
    }

    /// <summary>Verdadero cuando hay items y todos están marcados.</summary>
    public bool IsFinished => ItemCount > 0 && _items.All(i => i.Done);

    public IReadOnlyList<ChecklistItem> Items => _items;

    public IReadOnlyList<Correction> Corrections => _corrections;

    public int ItemCount => _items.Count;

    public int CorrectionCount => _corrections.Count;

    public event Action? NameChanged;
    public event Action? CategoryChanged;
    public event Action? ItemCountChanged;
    public event Action? CorrectionCountChanged;
    public event Action? ItemsChanged;
    public event Action? CorrectionsChanged;

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

    public void AddCorrection(Correction correction)
    {
        if (correction is null) return;
        _corrections.Add(correction);
        CorrectionCountChanged?.Invoke();
        CorrectionsChanged?.Invoke();
    }

    public void RemoveCorrection(int index)
    {
        if (index < 0 || index >= _corrections.Count) return;
        _corrections.RemoveAt(index);
        CorrectionCountChanged?.Invoke();
        CorrectionsChanged?.Invoke();
    }

    public Correction? GetCorrection(int index)
        => index < 0 || index >= _corrections.Count ? null : _corrections[index];

    public void AddNewItem(string text) => AddItem(new ChecklistItem(text, false));

    public Correction AddNewCorrection(string name)
    {
        var correction = new Correction(name);
        AddCorrection(correction);
        return correction;
    }

    private void OnPropertyChanged([CallerMemberName] string? propertyName = null)
        => PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(propertyName));
}