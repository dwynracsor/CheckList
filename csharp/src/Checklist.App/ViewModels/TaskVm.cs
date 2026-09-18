using System;
using System.Collections.ObjectModel;
using System.Linq;
using System.Windows.Input;
using Checklist.Controllers;

namespace Checklist.App.ViewModels;

/// <summary>
/// Vista de una lista ("task") dentro de "My lists": cabecera con nombre
/// editable, items, correcciones, barra de progreso y fecha.
/// </summary>
public sealed class TaskVm : ObservableObject
{
    private readonly TaskController _controller;
    private readonly int _taskIndex;
    private readonly Checklist.Domain.Task _task;

    private bool _isCollapsed;
    private bool _isEditingName;
    private bool _isEditingCategory;
    private bool _isAddingItem;
    private string _editName = string.Empty;
    private string _editCategory = string.Empty;
    private string _newItemText = string.Empty;

    private readonly Action? _onStateChanged;
    private readonly Action<string>? _onCopyText;

    public TaskVm(
        TaskController controller,
        int taskIndex,
        Action? onStateChanged = null,
        Action<string>? onCopyText = null)
    {
        _controller = controller;
        _taskIndex = taskIndex;
        _task = controller.GetTask(taskIndex)
                ?? throw new System.ArgumentOutOfRangeException(nameof(taskIndex));
        _onStateChanged = onStateChanged;
        _onCopyText = onCopyText;

        Items = new ObservableCollection<ItemVm>();
        Corrections = new ObservableCollection<CorrectionVm>();

        ToggleCollapseCommand = new RelayCommand(_ => IsCollapsed = !IsCollapsed);
        DeleteCommand = new RelayCommand(_ => _controller.RemoveTask(_taskIndex));
        ToggleRenameCommand = new RelayCommand(_ => ToggleRename());
        CommitRenameCommand = new RelayCommand(_ => CommitRename());
        CancelRenameCommand = new RelayCommand(_ => IsEditingName = false);
        ToggleCategoryCommand = new RelayCommand(_ => ToggleCategory());
        CommitCategoryCommand = new RelayCommand(_ => CommitCategory());
        CancelCategoryCommand = new RelayCommand(_ => IsEditingCategory = false);
        ToggleAddItemCommand = new RelayCommand(_ => IsAddingItem = !IsAddingItem);
        AddItemCommand = new RelayCommand(_ => AddItem());

        Refresh();
    }

    public ObservableCollection<ItemVm> Items { get; }

    public ObservableCollection<CorrectionVm> Corrections { get; }

    public string Name => _task.Name;

    public string EditName
    {
        get => _isEditingName ? _editName : Name;
        set => _editName = value;
    }

    public bool IsEditingName
    {
        get => _isEditingName;
        set
        {
            if (SetProperty(ref _isEditingName, value))
                if (value) _editName = _task.Name;
        }
    }

    public string CreatedAtText => Formatter.ShortDate(_task.Date);

    public DateOnly Date => _task.Date;

    public int CompletedCount => Items.Count(i => i.IsCompleted);

    public int ProgressMax => Items.Count == 0 ? 1 : Items.Count;

    /// <summary>Progreso redondeado (0 si no hay items).</summary>
    public int ProgressPercent => Items.Count == 0
        ? 0
        : (int)Math.Round(CompletedCount * 100.0 / Items.Count);

    // ---------- Tema / categoría ----------

    public string Category => _task.Category;

    public bool HasCategory => _task.Category.Length > 0;

    public string EditCategory
    {
        get => _isEditingCategory ? _editCategory : Category;
        set => _editCategory = value;
    }

    public bool IsEditingCategory
    {
        get => _isEditingCategory;
        set
        {
            if (SetProperty(ref _isEditingCategory, value))
                if (value) _editCategory = _task.Category;
        }
    }

    public bool IsCollapsed
    {
        get => _isCollapsed;
        set
        {
            if (SetProperty(ref _isCollapsed, value))
                Raise(nameof(CollapseGlyph));
        }
    }

    public string CollapseGlyph => IsCollapsed ? "+" : "\u2212";

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

    public bool HasCorrections => Corrections.Count > 0;

    public ICommand ToggleCollapseCommand { get; }
    public ICommand DeleteCommand { get; }
    public ICommand ToggleRenameCommand { get; }
    public ICommand CommitRenameCommand { get; }
    public ICommand CancelRenameCommand { get; }
    public ICommand ToggleCategoryCommand { get; }
    public ICommand CommitCategoryCommand { get; }
    public ICommand CancelCategoryCommand { get; }
    public ICommand ToggleAddItemCommand { get; }
    public ICommand AddItemCommand { get; }

    public void Refresh()
    {
        Items.Clear();
        for (var i = 0; i < _task.ItemCount; i++)
        {
            var item = _task.GetItem(i);
            if (item is null) continue;

            var itemIndex = i;
            Items.Add(new ItemVm(
                item,
                onDelete: _ =>
                {
                    _controller.RemoveItemFromTask(_taskIndex, itemIndex);
                    Refresh();
                },
                onToggle: (_, done) => _controller.ToggleItemInTask(_taskIndex, itemIndex, done),
                onCompletedChanged: _ => RaiseProgress(),
                onTextEdited: text => _controller.SetItemTextInTask(_taskIndex, itemIndex, text),
                onCopy: _onCopyText));
        }

        Corrections.Clear();
        for (var i = 0; i < _task.CorrectionCount; i++)
        {
            var correction = _task.GetCorrection(i);
            if (correction is null) continue;
            Corrections.Add(new CorrectionVm(_controller, _taskIndex, i, correction, _onCopyText));
        }

        RaiseProgress();
        Raise(nameof(HasCorrections));
        Raise(nameof(Name));
        Raise(nameof(Category));
        Raise(nameof(HasCategory));
        _onStateChanged?.Invoke();
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
        if (trimmed.Length > 0 && trimmed != _task.Name)
            _controller.SetTaskName(_taskIndex, trimmed);

        IsEditingName = false;
        Raise(nameof(Name));
        Raise(nameof(EditName));
        _onStateChanged?.Invoke();
    }

    private void ToggleCategory()
    {
        if (IsEditingCategory)
        {
            CommitCategory();
        }
        else
        {
            IsEditingCategory = true;
            Raise(nameof(EditCategory));
        }
    }

    private void CommitCategory()
    {
        var trimmed = EditCategory.Trim();
        if (trimmed != _task.Category)
            _controller.SetTaskCategory(_taskIndex, trimmed);

        IsEditingCategory = false;
        Raise(nameof(Category));
        Raise(nameof(HasCategory));
        Raise(nameof(EditCategory));
        _onStateChanged?.Invoke();
    }

    private void AddItem()
    {
        var text = NewItemText.Trim();
        if (text.Length == 0) return;

        _controller.AddItemToTask(_taskIndex, text);
        NewItemText = string.Empty;
        Refresh();
    }

    private void RaiseProgress()
    {
        Raise(nameof(CompletedCount));
        Raise(nameof(ProgressMax));
        Raise(nameof(ProgressPercent));
        _onStateChanged?.Invoke();
    }
}