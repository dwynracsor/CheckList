using System.ComponentModel;
using System.Runtime.CompilerServices;

namespace Checklist.Domain;

public class ChecklistItem : INotifyPropertyChanged
{
    private string _text;
    private bool _done;

    public event PropertyChangedEventHandler? PropertyChanged;

    public ChecklistItem() : this(Guid.NewGuid().ToString("N"), "", false)
    {
    }

    public ChecklistItem(string text, bool done = false)
        : this(Guid.NewGuid().ToString("N"), text, done)
    {
    }

    public ChecklistItem(string id, string text, bool done)
    {
        Id = string.IsNullOrEmpty(id) ? Guid.NewGuid().ToString("N") : id;
        _text = text;
        _done = done;
    }

    public string Id { get; }

    public string Text
    {
        get => _text;
        set
        {
            if (_text != value)
            {
                _text = value;
                OnPropertyChanged();
                TextChanged?.Invoke();
            }
        }
    }

    public bool Done
    {
        get => _done;
        set
        {
            if (_done != value)
            {
                _done = value;
                OnPropertyChanged();
                DoneChanged?.Invoke();
            }
        }
    }

    public event Action? TextChanged;
    public event Action? DoneChanged;

    public void Toggle() => Done = !Done;

    private void OnPropertyChanged([CallerMemberName] string? propertyName = null)
        => PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(propertyName));
}