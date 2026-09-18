using System;
using System.Windows.Input;

namespace Checklist.App.ViewModels;

/// <summary>
/// Vista de un item (tarea) de una lista o corrección. Delega en el
/// controlador para mantener undo/redo y autosave; el modelo de dominio es la
/// fuente de verdad del texto y del estado completado.
/// </summary>
public sealed class ItemVm : ObservableObject
{
    private readonly Checklist.Domain.ChecklistItem _domain;
    private readonly Action<ItemVm> _onDelete;
    private readonly Action<ItemVm, bool> _onToggle;
    private readonly Action<ItemVm>? _onCompletedChanged;
    private readonly Action<string>? _onTextEdited;
    private readonly Action<string>? _onCopy;

    private bool _isEditing;
    private string _editText = string.Empty;

    public ItemVm(
        Checklist.Domain.ChecklistItem domain,
        Action<ItemVm> onDelete,
        Action<ItemVm, bool> onToggle,
        Action<ItemVm>? onCompletedChanged = null,
        Action<string>? onTextEdited = null,
        Action<string>? onCopy = null)
    {
        _domain = domain;
        _onDelete = onDelete;
        _onToggle = onToggle;
        _onCompletedChanged = onCompletedChanged;
        _onTextEdited = onTextEdited;
        _onCopy = onCopy;

        DeleteCommand = new RelayCommand(_ => _onDelete(this));
        CopyCommand = new RelayCommand(_ => _onCopy?.Invoke(Text));
        EditCommand = new RelayCommand(_ => BeginEdit());
        CommitEditCommand = new RelayCommand(_ => CommitEdit());
        CancelEditCommand = new RelayCommand(_ => IsEditing = false);
    }

    public string Text => _domain.Text;

    public bool IsCompleted
    {
        get => _domain.Done;
        set
        {
            if (_domain.Done == value) return;
            _onToggle(this, value);
            Raise();
            _onCompletedChanged?.Invoke(this);
        }
    }

    // ---------- Edición inline (menú contextual "Edit item") ----------

    public bool IsEditing
    {
        get => _isEditing;
        set => SetProperty(ref _isEditing, value);
    }

    public string EditText
    {
        get => _isEditing ? _editText : Text;
        set => _editText = value;
    }

    public ICommand DeleteCommand { get; }
    public ICommand CopyCommand { get; }
    public ICommand EditCommand { get; }
    public ICommand CommitEditCommand { get; }
    public ICommand CancelEditCommand { get; }

    private void BeginEdit()
    {
        _editText = Text;
        IsEditing = true;
        Raise(nameof(EditText));
    }

    public void CommitEdit()
    {
        var trimmed = EditText.Trim();
        if (IsEditing && trimmed.Length > 0 && trimmed != Text)
            _onTextEdited?.Invoke(trimmed);

        IsEditing = false;
        Raise(nameof(Text));
        Raise(nameof(EditText));
    }
}