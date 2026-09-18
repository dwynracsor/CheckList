using System.ComponentModel;
using System.Runtime.CompilerServices;
using Checklist.Domain;
using Checklist.Persistence;
using Checklist.Undo;

namespace Checklist.Controllers;

public class TaskController : INotifyPropertyChanged, IDisposable
{
    private readonly List<Task> _tasks = new();
    private readonly Stack<UndoCommand> _undoStack = new();
    private readonly Stack<UndoCommand> _redoStack = new();
    private readonly object _autoSaveLock = new();
    private CancellationTokenSource? _autoSaveCts;
    private bool _isLoaded;
    private bool _autoSave = true;
    private int _autoSaveDelay = 500;
    private string _lastError = "";

    public TaskController()
        : this(new JsonStorage())
    {
    }

    public TaskController(string storagePath)
        : this(new JsonStorage(storagePath))
    {
    }

    private TaskController(JsonStorage storage)
    {
        Storage = storage;
        Storage.Error += SetError;
    }

    public JsonStorage Storage { get; }

    public IReadOnlyList<Task> Tasks => _tasks;

    public int TaskCount => _tasks.Count;

    public bool IsLoaded => _isLoaded;

    public string LastError => _lastError;

    public bool AutoSave
    {
        get => _autoSave;
        set
        {
            if (_autoSave != value)
            {
                _autoSave = value;
                AutoSaveChanged?.Invoke();
                if (!_autoSave)
                {
                    lock (_autoSaveLock) _autoSaveCts?.Cancel();
                }
            }
        }
    }

    public int AutoSaveDelay
    {
        get => _autoSaveDelay;
        set
        {
            if (_autoSaveDelay != value)
            {
                _autoSaveDelay = value;
                AutoSaveDelayChanged?.Invoke();
            }
        }
    }

    public bool CanUndo => _undoStack.Count > 0;

    public bool CanRedo => _redoStack.Count > 0;

    public event Action? TasksChanged;
    public event Action? LoadedChanged;
    public event Action? ErrorChanged;
    public event Action? AutoSaveChanged;
    public event Action? AutoSaveDelayChanged;
    public event Action? UndoStackChanged;
    public event Action? RedoStackChanged;
    public event Action? Saved;
    public event Action<int>? TaskAdded;
    public event Action<int>? TaskRemoved;

    public event PropertyChangedEventHandler? PropertyChanged;

    // --- Core ---

    public void Load()
    {
        lock (_autoSaveLock) _autoSaveCts?.Cancel();

        var loaded = Storage.Load();
        _tasks.Clear();
        _tasks.AddRange(loaded);

        if (!_isLoaded)
        {
            _isLoaded = true;
            LoadedChanged?.Invoke();
        }

        TasksChanged?.Invoke();
    }

    public bool Save()
    {
        if (Storage.Save(_tasks))
        {
            Saved?.Invoke();
            return true;
        }
        return false;
    }

    // --- Task operations ---

    public Task? AddTask(string name, string category = "")
    {
        var trimmed = name?.Trim() ?? "";
        var trimmedCategory = category?.Trim() ?? "";
        if (string.IsNullOrEmpty(trimmed))
        {
            SetError("Task name cannot be empty");
            return null;
        }

        var task = new Task(trimmed, DateOnly.FromDateTime(DateTime.Today))
        {
            Category = trimmedCategory
        };
        var newIndex = _tasks.Count;
        _tasks.Add(task);

        TasksChanged?.Invoke();
        TaskAdded?.Invoke(newIndex);
        ScheduleAutoSave();

        PushUndo(new UndoCommand(
            "Add task: " + trimmed,
            undo: () =>
            {
                if (newIndex >= 0 && newIndex < _tasks.Count)
                {
                    _tasks.RemoveAt(newIndex);
                    TasksChanged?.Invoke();
                }
            },
            redo: () =>
            {
                var t = new Task(trimmed, DateOnly.FromDateTime(DateTime.Today))
                {
                    Category = trimmedCategory
                };
                if (newIndex >= _tasks.Count) _tasks.Add(t);
                else _tasks.Insert(newIndex, t);
                TasksChanged?.Invoke();
                TaskAdded?.Invoke(newIndex);
            }));

        return task;
    }

    public void RemoveTask(int index)
    {
        if (index < 0 || index >= _tasks.Count) return;

        var task = _tasks[index];
        var taskName = task.Name;
        int capturedIndex = index;

        _tasks.RemoveAt(index);
        TasksChanged?.Invoke();
        TaskRemoved?.Invoke(index);
        ScheduleAutoSave();

        PushUndo(new UndoCommand(
            "Delete task: " + taskName,
            undo: () =>
            {
                if (capturedIndex >= _tasks.Count) _tasks.Add(task);
                else _tasks.Insert(capturedIndex, task);
                TasksChanged?.Invoke();
                TaskAdded?.Invoke(capturedIndex);
            },
            redo: () =>
            {
                var idx = _tasks.IndexOf(task);
                if (idx >= 0)
                {
                    _tasks.RemoveAt(idx);
                    TasksChanged?.Invoke();
                    TaskRemoved?.Invoke(capturedIndex);
                }
            }));
    }

    public Task? GetTask(int index)
        => index < 0 || index >= _tasks.Count ? null : _tasks[index];

    // --- Item operations on task ---

    public void AddItemToTask(int taskIndex, string text)
    {
        var task = GetTask(taskIndex);
        var trimmed = text?.Trim() ?? "";
        if (task is null || string.IsNullOrEmpty(trimmed)) return;

        task.AddNewItem(trimmed);
        var itemIndex = task.ItemCount - 1;
        ScheduleAutoSave();

        PushUndo(new UndoCommand(
            "Add item",
            undo: () =>
            {
                var t = GetTask(taskIndex);
                if (t is not null && itemIndex < t.ItemCount)
                {
                    t.RemoveItem(itemIndex);
                    TasksChanged?.Invoke();
                }
            },
            redo: () =>
            {
                var t = GetTask(taskIndex);
                if (t is not null)
                {
                    t.AddNewItem(trimmed);
                    TasksChanged?.Invoke();
                }
            }));
    }

    public void RemoveItemFromTask(int taskIndex, int itemIndex)
    {
        var task = GetTask(taskIndex);
        if (task is null) return;

        var item = task.GetItem(itemIndex);
        if (item is null) return;

        var itemText = item.Text;
        var itemDone = item.Done;

        task.RemoveItem(itemIndex);
        ScheduleAutoSave();

        PushUndo(new UndoCommand(
            "Delete item: " + itemText,
            undo: () =>
            {
                var t = GetTask(taskIndex);
                if (t is not null)
                {
                    t.AddItem(new ChecklistItem(itemText, itemDone));
                    TasksChanged?.Invoke();
                }
            },
            redo: () =>
            {
                var t = GetTask(taskIndex);
                if (t is not null && itemIndex < t.ItemCount)
                {
                    t.RemoveItem(itemIndex);
                    TasksChanged?.Invoke();
                }
            }));
    }

    public void ToggleItemInTask(int taskIndex, int itemIndex, bool done)
    {
        var task = GetTask(taskIndex);
        if (task is null) return;

        var item = task.GetItem(itemIndex);
        if (item is null) return;

        var previousDone = item.Done;
        item.Done = done;
        ScheduleAutoSave();

        PushUndo(new UndoCommand(
            done ? "Check item" : "Uncheck item",
            undo: () =>
            {
                var t = GetTask(taskIndex);
                if (t?.GetItem(itemIndex) is { } i) i.Done = previousDone;
            },
            redo: () =>
            {
                var t = GetTask(taskIndex);
                if (t?.GetItem(itemIndex) is { } i) i.Done = done;
            }));
    }

    public void SetItemTextInTask(int taskIndex, int itemIndex, string text)
    {
        var task = GetTask(taskIndex);
        if (task is null) return;

        var item = task.GetItem(itemIndex);
        if (item is null) return;

        var newText = text?.Trim() ?? "";
        var oldText = item.Text;
        if (oldText == newText || string.IsNullOrEmpty(newText)) return;

        item.Text = newText;
        ScheduleAutoSave();

        PushUndo(new UndoCommand(
            "Edit item",
            undo: () =>
            {
                var t = GetTask(taskIndex);
                if (t?.GetItem(itemIndex) is { } i) i.Text = oldText;
            },
            redo: () =>
            {
                var t = GetTask(taskIndex);
                if (t?.GetItem(itemIndex) is { } i) i.Text = newText;
            }));
    }

    public void SetItemTextInCorrection(int taskIndex, int correctionIndex, int itemIndex, string text)
    {
        var task = GetTask(taskIndex);
        if (task is null) return;

        var corr = task.GetCorrection(correctionIndex);
        if (corr is null) return;

        var item = corr.GetItem(itemIndex);
        if (item is null) return;

        var newText = text?.Trim() ?? "";
        var oldText = item.Text;
        if (oldText == newText || string.IsNullOrEmpty(newText)) return;

        item.Text = newText;
        ScheduleAutoSave();

        PushUndo(new UndoCommand(
            "Edit correction item",
            undo: () =>
            {
                var t = GetTask(taskIndex);
                if (t?.GetCorrection(correctionIndex)?.GetItem(itemIndex) is { } i) i.Text = oldText;
            },
            redo: () =>
            {
                var t = GetTask(taskIndex);
                if (t?.GetCorrection(correctionIndex)?.GetItem(itemIndex) is { } i) i.Text = newText;
            }));
    }

    // --- Correction operations on task ---

    public Correction? AddCorrectionToTask(int taskIndex, string name)
    {
        var task = GetTask(taskIndex);
        var trimmed = name?.Trim() ?? "";
        if (task is null || string.IsNullOrEmpty(trimmed)) return null;

        var corr = task.AddNewCorrection(trimmed);
        var corrIndex = task.CorrectionCount - 1;
        ScheduleAutoSave();

        PushUndo(new UndoCommand(
            "Add correction: " + trimmed,
            undo: () =>
            {
                var t = GetTask(taskIndex);
                if (t is not null && corrIndex < t.CorrectionCount)
                {
                    t.RemoveCorrection(corrIndex);
                    TasksChanged?.Invoke();
                }
            },
            redo: () =>
            {
                var t = GetTask(taskIndex);
                if (t is not null)
                {
                    t.AddNewCorrection(trimmed);
                    TasksChanged?.Invoke();
                }
            }));

        return corr;
    }

    public void RemoveCorrectionFromTask(int taskIndex, int correctionIndex)
    {
        var task = GetTask(taskIndex);
        if (task is null) return;

        var corr = task.GetCorrection(correctionIndex);
        if (corr is null) return;

        var corrName = corr.Name;
        task.RemoveCorrection(correctionIndex);
        ScheduleAutoSave();

        PushUndo(new UndoCommand(
            "Delete correction: " + corrName,
            undo: () =>
            {
                var t = GetTask(taskIndex);
                if (t is not null)
                {
                    t.AddCorrection(new Correction(corrName, DateOnly.FromDateTime(DateTime.Today)));
                    TasksChanged?.Invoke();
                }
            },
            redo: () =>
            {
                var t = GetTask(taskIndex);
                if (t is not null && correctionIndex < t.CorrectionCount)
                {
                    t.RemoveCorrection(correctionIndex);
                    TasksChanged?.Invoke();
                }
            }));
    }

    // --- Item operations on correction ---

    public void AddItemToCorrection(int taskIndex, int correctionIndex, string text)
    {
        var task = GetTask(taskIndex);
        var trimmed = text?.Trim() ?? "";
        if (task is null || string.IsNullOrEmpty(trimmed)) return;

        var corr = task.GetCorrection(correctionIndex);
        if (corr is null) return;

        corr.AddNewItem(trimmed);
        var itemIndex = corr.ItemCount - 1;
        ScheduleAutoSave();

        PushUndo(new UndoCommand(
            "Add item to correction",
            undo: () =>
            {
                var t = GetTask(taskIndex);
                var c = t?.GetCorrection(correctionIndex);
                if (c is not null && itemIndex < c.ItemCount)
                {
                    c.RemoveItem(itemIndex);
                    TasksChanged?.Invoke();
                }
            },
            redo: () =>
            {
                var t = GetTask(taskIndex);
                var c = t?.GetCorrection(correctionIndex);
                if (c is not null)
                {
                    c.AddNewItem(trimmed);
                    TasksChanged?.Invoke();
                }
            }));
    }

    public void RemoveItemFromCorrection(int taskIndex, int correctionIndex, int itemIndex)
    {
        var task = GetTask(taskIndex);
        if (task is null) return;

        var corr = task.GetCorrection(correctionIndex);
        if (corr is null) return;

        var item = corr.GetItem(itemIndex);
        if (item is null) return;

        var itemText = item.Text;
        var itemDone = item.Done;

        corr.RemoveItem(itemIndex);
        ScheduleAutoSave();

        PushUndo(new UndoCommand(
            "Delete correction item: " + itemText,
            undo: () =>
            {
                var t = GetTask(taskIndex);
                var c = t?.GetCorrection(correctionIndex);
                if (c is not null)
                {
                    c.AddItem(new ChecklistItem(itemText, itemDone));
                    TasksChanged?.Invoke();
                }
            },
            redo: () =>
            {
                var t = GetTask(taskIndex);
                var c = t?.GetCorrection(correctionIndex);
                if (c is not null && itemIndex < c.ItemCount)
                {
                    c.RemoveItem(itemIndex);
                    TasksChanged?.Invoke();
                }
            }));
    }

    public void ToggleItemInCorrection(int taskIndex, int correctionIndex, int itemIndex, bool done)
    {
        var task = GetTask(taskIndex);
        if (task is null) return;

        var corr = task.GetCorrection(correctionIndex);
        if (corr is null) return;

        var item = corr.GetItem(itemIndex);
        if (item is null) return;

        var previousDone = item.Done;
        item.Done = done;
        ScheduleAutoSave();

        PushUndo(new UndoCommand(
            done ? "Check correction item" : "Uncheck correction item",
            undo: () =>
            {
                var t = GetTask(taskIndex);
                var c = t?.GetCorrection(correctionIndex);
                if (c?.GetItem(itemIndex) is { } i) i.Done = previousDone;
            },
            redo: () =>
            {
                var t = GetTask(taskIndex);
                var c = t?.GetCorrection(correctionIndex);
                if (c?.GetItem(itemIndex) is { } i) i.Done = done;
            }));
    }

    // --- Name editing ---

    public void SetTaskName(int taskIndex, string name)
    {
        var task = GetTask(taskIndex);
        var trimmed = name?.Trim() ?? "";
        if (task is null || string.IsNullOrEmpty(trimmed)) return;

        var oldName = task.Name;
        task.Name = trimmed;
        ScheduleAutoSave();

        PushUndo(new UndoCommand(
            "Rename task",
            undo: () =>
            {
                var t = GetTask(taskIndex);
                if (t is not null) t.Name = oldName;
            },
            redo: () =>
            {
                var t = GetTask(taskIndex);
                if (t is not null) t.Name = trimmed;
            }));
    }

    public void SetCorrectionName(int taskIndex, int correctionIndex, string name)
    {
        var task = GetTask(taskIndex);
        var trimmed = name?.Trim() ?? "";
        if (task is null || string.IsNullOrEmpty(trimmed)) return;

        var corr = task.GetCorrection(correctionIndex);
        if (corr is null) return;

        var oldName = corr.Name;
        corr.Name = trimmed;
        ScheduleAutoSave();

        PushUndo(new UndoCommand(
            "Rename correction",
            undo: () =>
            {
                var t = GetTask(taskIndex);
                if (t?.GetCorrection(correctionIndex) is { } c) c.Name = oldName;
            },
            redo: () =>
            {
                var t = GetTask(taskIndex);
                if (t?.GetCorrection(correctionIndex) is { } c) c.Name = trimmed;
            }));
    }

    public void SetTaskCategory(int taskIndex, string category)
    {
        var task = GetTask(taskIndex);
        var trimmed = category?.Trim() ?? "";
        if (task is null) return;

        var oldCategory = task.Category;
        if (trimmed == oldCategory) return;

        task.Category = trimmed;
        TasksChanged?.Invoke();
        ScheduleAutoSave();

        PushUndo(new UndoCommand(
            "Change task theme",
            undo: () =>
            {
                var t = GetTask(taskIndex);
                if (t is not null) t.Category = oldCategory;
            },
            redo: () =>
            {
                var t = GetTask(taskIndex);
                if (t is not null) t.Category = trimmed;
            }));
    }

    // --- Import from web format ---

    public void ImportFromJsonString(string jsonString)
    {
        var tasks = JsonStorage.TryParseIntoArray(jsonString, out var error);
        if (tasks is null)
        {
            SetError("Error parsing JSON: " + error);
            return;
        }

        var imported = 0;
        foreach (var json in tasks)
        {
            var task = JsonStorage.JsonToTask(json);
            if (task is not null)
            {
                _tasks.Add(task);
                imported++;
            }
        }

        if (imported > 0)
        {
            TasksChanged?.Invoke();
            ScheduleAutoSave();
        }
    }

    // --- Undo / Redo ---

    public void Undo()
    {
        if (_undoStack.Count == 0) return;

        var cmd = _undoStack.Pop();
        cmd.Undo();
        _redoStack.Push(cmd);

        UndoStackChanged?.Invoke();
        RedoStackChanged?.Invoke();
        TasksChanged?.Invoke();
        ScheduleAutoSave();
    }

    public void Redo()
    {
        if (_redoStack.Count == 0) return;

        var cmd = _redoStack.Pop();
        cmd.Redo();
        _undoStack.Push(cmd);

        UndoStackChanged?.Invoke();
        RedoStackChanged?.Invoke();
        TasksChanged?.Invoke();
        ScheduleAutoSave();
    }

    public void ClearHistory()
    {
        _undoStack.Clear();
        _redoStack.Clear();
        UndoStackChanged?.Invoke();
        RedoStackChanged?.Invoke();
    }

    private void PushUndo(UndoCommand cmd)
    {
        _undoStack.Push(cmd);
        _redoStack.Clear();
        UndoStackChanged?.Invoke();
        RedoStackChanged?.Invoke();
    }

    // --- Private ---

    private void ScheduleAutoSave()
    {
        if (!_autoSave) return;

        lock (_autoSaveLock)
        {
            _autoSaveCts?.Cancel();
            var cts = _autoSaveCts = new CancellationTokenSource();
            _ = DebounceSaveAsync(cts.Token);
        }
    }

    private async System.Threading.Tasks.Task DebounceSaveAsync(CancellationToken token)
    {
        try
        {
            await System.Threading.Tasks.Task.Delay(_autoSaveDelay, token);
        }
        catch (TaskCanceledException)
        {
            return;
        }

        Save();
    }

    private void SetError(string message)
    {
        if (_lastError != message)
        {
            _lastError = message;
            ErrorChanged?.Invoke();
            PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(nameof(LastError)));
        }
    }

    private void OnPropertyChanged([CallerMemberName] string? propertyName = null)
        => PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(propertyName));

    public void Dispose()
    {
        lock (_autoSaveLock)
        {
            _autoSaveCts?.Cancel();
            _autoSaveCts?.Dispose();
        }
        GC.SuppressFinalize(this);
    }
}