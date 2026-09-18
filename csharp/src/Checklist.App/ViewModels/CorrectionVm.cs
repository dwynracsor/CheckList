using System;
using System.Collections.ObjectModel;
using System.Windows.Input;
using Checklist.Controllers;
using Checklist.Domain;

namespace Checklist.App.ViewModels;

/// <summary>
/// Vista de una corrección dentro de una tarjeta: nombre editable, sus items
/// y su propio "x" de eliminación.
/// </summary>
public sealed class CorrectionVm : ObservableObject
{
    private readonly TaskController _controller;
    private readonly int _taskIndex;
    private readonly int _correctionIndex;
    private readonly Correction _correction;
    private readonly Action<string>? _onCopyText;

    private bool _isEditingName;
    private bool _isAddingItem;
    private string _newItemText = string.Empty;

    public CorrectionVm(
        TaskController controller,
        int taskIndex,
        int correctionIndex,
        Correction correction,
        Action<string>? onCopyText = null)
    {
        _controller = controller;
        _taskIndex = taskIndex;
        _correctionIndex = correctionIndex;
        _correction = correction;
        _onCopyText = onCopyText;

        Items = new ObservableCollection<ItemVm>();

        ToggleRenameCommand = new RelayCommand(_ => ToggleRename());
        CommitRenameCommand = new RelayCommand(_ => CommitRename());
        CancelRenameCommand = new RelayCommand(_ => IsEditingName = false);
        DeleteCommand = new RelayCommand(_ =>
        {
            _controller.RemoveCorrectionFromTask(_taskIndex, _correctionIndex);
            Refresh();
        });
        ToggleAddItemCommand = new RelayCommand(_ => IsAddingItem = !IsAddingItem);
        AddItemCommand = new RelayCommand(_ => AddItem());

        Refresh();
    }

    public ObservableCollection<ItemVm> Items { get; }

    public string Name => _correction.Name;

    public string EditName
    {
        get => _isEditingName ? _editName : Name;
        set => _editName = value;
    }

    private string _editName = string.Empty;

    public bool IsEditingName
    {
        get => _isEditingName;
        set
        {
            if (SetProperty(ref _isEditingName, value))
                if (value) _editName = _correction.Name;
        }
    }

    public bool IsAddingItem
    {
        get => _isAddingItem;
        set => SetProperty(ref _isAddingItem, value);
    }

    public string NewItemText
    {
        get => _newItemText;
        set => SetProperty(ref _newItemText, value);
    }

    public ICommand ToggleRenameCommand { get; }
    public ICommand CommitRenameCommand { get; }
    public ICommand CancelRenameCommand { get; }
    public ICommand DeleteCommand { get; }
    public ICommand ToggleAddItemCommand { get; }
    public ICommand AddItemCommand { get; }

    public void Refresh()
    {
        Items.Clear();
        for (var i = 0; i < _correction.ItemCount; i++)
        {
            var item = _correction.GetItem(i);
            if (item is null) continue;

            var itemIndex = i;
            Items.Add(new ItemVm(
                item,
                onDelete: _ =>
                {
                    _controller.RemoveItemFromCorrection(_taskIndex, _correctionIndex, itemIndex);
                    Refresh();
                },
                onToggle: (_, done) => _controller.ToggleItemInCorrection(_taskIndex, _correctionIndex, itemIndex, done),
                onTextEdited: text => _controller.SetItemTextInCorrection(_taskIndex, _correctionIndex, itemIndex, text),
                onCopy: _onCopyText));
        }

        Raise(nameof(Name));
    }

    private void ToggleRename()
    {
        if (IsEditingName)
        {
            CommitRename();
        }
        else
        {
            IsEditingName = true;
            Raise(nameof(EditName));
        }
    }

    private void CommitRename()
    {
        var trimmed = EditName.Trim();
        if (trimmed.Length > 0 && trimmed != _correction.Name)
            _controller.SetCorrectionName(_taskIndex, _correctionIndex, trimmed);

        IsEditingName = false;
        Raise(nameof(Name));
        Raise(nameof(EditName));
    }

    private void AddItem()
    {
        var text = NewItemText.Trim();
        if (text.Length == 0) return;

        _controller.AddItemToCorrection(_taskIndex, _correctionIndex, text);
        NewItemText = string.Empty;
        Refresh();
    }
}