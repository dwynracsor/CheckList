using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Collections.Specialized;
using System.Linq;
using System.Windows.Input;
using Avalonia.Input;
using Avalonia.Input.Platform;
using Avalonia.Threading;
using Checklist.Controllers;
using Checklist.Persistence;

namespace Checklist.App.ViewModels;

/// <summary>
/// Estado de la ventana completa: cabecera, búsqueda, filtros, grupo de listas,
/// panel "Create list" y el listado "My lists" agrupado por fecha o por tema.
/// El controlador es la fuente de verdad; este VM solo espeja su estado para la vista.
/// </summary>
public sealed class MainViewModel : ObservableObject, IDisposable
{
    public const int ReminderThresholdDays = 7;

    private readonly TaskController _controller = new();
    private readonly SettingsStore _settings = new();
    private readonly DispatcherTimer _alertTimer;
    private readonly HashSet<string> _dismissedAlertIds = new();

    private IClipboard? _clipboard;
    private string _lastUsedCategory = string.Empty;
    private string _selectedDraftTheme = string.Empty;

    private string _searchText = string.Empty;
    private string _draftName = string.Empty;
    private string _draftCategory = string.Empty;
    private string _draftItemText = string.Empty;
    private bool _isCreateExpanded = true;
    private bool _isDarkMode;
    private bool _isNotificationsOpen;
    private int _groupModeIndex;
    private int _dateFilterIndex;
    private DateTime? _filterFrom;
    private DateTime? _filterTo;
    private bool _disposed;

    public MainViewModel()
    {
        DraftItems = new ObservableCollection<DraftItemVm>();
        DraftItems.CollectionChanged += OnDraftItemsChanged;

        Groups = new ObservableCollection<GroupVm>();
        Alerts = new ObservableCollection<AlertVm>();
        DraftThemeOptions = new ObservableCollection<string>();

        AddDraftItemCommand = new RelayCommand(_ => AddDraftItem());
        CreateListCommand = new RelayCommand(_ => CreateList(), _ => CanCreate);
        ToggleCreateCommand = new RelayCommand(_ => IsCreateExpanded = !IsCreateExpanded);
        ToggleNotificationsCommand = new RelayCommand(_ => IsNotificationsOpen = !IsNotificationsOpen);

        _controller.TasksChanged += OnTasksChanged;
        _controller.Load();
        RefreshLists();

        _lastUsedCategory = _settings.LastCategory;
        DraftCategory = _lastUsedCategory;

        _alertTimer = new DispatcherTimer { Interval = TimeSpan.FromMinutes(1) };
        _alertTimer.Tick += (_, _) => RebuildAlerts();
        _alertTimer.Start();
    }

    public TaskController Controller => _controller;

    /// <summary>Listas agrupadas según <see cref="GroupModeIndex"/>.</summary>
    public ObservableCollection<GroupVm> Groups { get; }

    public ObservableCollection<DraftItemVm> DraftItems { get; }

    public ObservableCollection<AlertVm> Alerts { get; }

    /// <summary>Temas ya existentes en las listas, para elegir al crear una nueva.</summary>
    public ObservableCollection<string> DraftThemeOptions { get; }

    // ---------- Cabecera ----------

    public bool IsDarkMode
    {
        get => _isDarkMode;
        set => SetProperty(ref _isDarkMode, value);
    }

    /// <summary>Listas que están pendientes hace <see cref="ReminderThresholdDays"/> días o más.</summary>
    public int AlertCount => Alerts.Count;

    public bool HasAlerts => Alerts.Count > 0;

    public bool IsNotificationsOpen
    {
        get => _isNotificationsOpen;
        set => SetProperty(ref _isNotificationsOpen, value);
    }

    public ICommand ToggleNotificationsCommand { get; }

    /// <summary>Proporciona el portapapeles de la ventana (desde MainWindow).</summary>
    public void AttachClipboard(IClipboard? clipboard) => _clipboard = clipboard;

    private void CopyItemText(string text) => _ = _clipboard?.SetTextAsync(text);

    // ---------- Búsqueda ----------

    public string SearchText
    {
        get => _searchText;
        set
        {
            if (SetProperty(ref _searchText, value))
                RefreshLists();
        }
    }

    // ---------- Agrupación y filtros ----------

    public IReadOnlyList<string> GroupModeOptions { get; } = new[] { "By theme", "By date" };

    /// <summary>0 = por tema (por defecto), 1 = por fecha.</summary>
    public int GroupModeIndex
    {
        get => _groupModeIndex;
        set
        {
            if (SetProperty(ref _groupModeIndex, value))
                RefreshLists();
        }
    }

    public IReadOnlyList<string> DateFilterOptions { get; } = new[]
    {
        "All dates", "Today", "This week", "This month", "Custom range"
    };

    /// <summary>0 = todas, 1 = hoy, 2 = esta semana, 3 = este mes, 4 = rango.</summary>
    public int DateFilterIndex
    {
        get => _dateFilterIndex;
        set
        {
            if (SetProperty(ref _dateFilterIndex, value))
            {
                RefreshLists();
                Raise(nameof(IsCustomRange));
            }
        }
    }

    /// <summary>True cuando el selector de rango está visible (filtro = rango personalizado).</summary>
    public bool IsCustomRange => DateFilterIndex == 4;

    public DateTime? FilterFrom
    {
        get => _filterFrom;
        set
        {
            if (SetProperty(ref _filterFrom, value) && DateFilterIndex == 4)
                RefreshLists();
        }
    }

    public DateTime? FilterTo
    {
        get => _filterTo;
        set
        {
            if (SetProperty(ref _filterTo, value) && DateFilterIndex == 4)
                RefreshLists();
        }
    }

    public bool HasLists => Groups.Count > 0 && Groups.Any(g => g.Count > 0);

    // ---------- Panel "Create list" ----------

    public bool IsCreateExpanded
    {
        get => _isCreateExpanded;
        set
        {
            if (SetProperty(ref _isCreateExpanded, value))
                Raise(nameof(ChevronGlyph));
        }
    }

    public string ChevronGlyph => IsCreateExpanded ? "\u25BC" : "\u25B2";

    public string DraftName
    {
        get => _draftName;
        set
        {
            if (SetProperty(ref _draftName, value))
                RaiseCanCreate();
        }
    }

    public string DraftCategory
    {
        get => _draftCategory;
        set
        {
            if (SetProperty(ref _draftCategory, value))
            {
                var trimmed = value?.Trim() ?? "";
                if (trimmed.Length > 0 && !string.Equals(trimmed, _lastUsedCategory, StringComparison.OrdinalIgnoreCase))
                {
                    _lastUsedCategory = trimmed;
                    _settings.LastCategory = trimmed;
                }
            }
        }
    }

    /// <summary>Selección del desplegable de temas existentes (elige y se limpia solo).</summary>
    public string SelectedDraftTheme
    {
        get => _selectedDraftTheme;
        set
        {
            if (value is null) return;
            if (value.Length == 0) return;

            DraftCategory = value;
            _selectedDraftTheme = string.Empty;
            Raise(nameof(SelectedDraftTheme));
        }
    }

    public string DraftItemText
    {
        get => _draftItemText;
        set => SetProperty(ref _draftItemText, value);
    }

    public bool CanCreate => DraftName.Trim().Length > 0 && DraftItems.Count > 0;

    public bool HasDraftItems => DraftItems.Count > 0;

    public ICommand AddDraftItemCommand { get; }

    public RelayCommand CreateListCommand { get; }

    public ICommand ToggleCreateCommand { get; }

    // ---------- Lógica ----------

    private void AddDraftItem()
    {
        var text = DraftItemText.Trim();
        if (text.Length == 0) return;

        DraftItems.Add(new DraftItemVm(text, item => DraftItems.Remove(item)));
        DraftItemText = string.Empty;
        TrySuggestCategoryFromItems();
    }

    /// <summary>
    /// Si el tema del borrador está vacío, lo rellena automáticamente cuando
    /// alguna palabra de los items coincide con un tema ya existente.
    /// </summary>
    private void TrySuggestCategoryFromItems()
    {
        if (DraftCategory.Trim().Length > 0) return;

        var itemWords = DraftItems
            .SelectMany(i => Tokenize(i.Text))
            .ToHashSet(StringComparer.OrdinalIgnoreCase);
        if (itemWords.Count == 0) return;

        foreach (var theme in _controller.Tasks
                     .Select(t => t.Category.Trim())
                     .Where(c => c.Length > 0)
                     .Distinct(StringComparer.OrdinalIgnoreCase))
        {
            foreach (var themeWord in Tokenize(theme))
            {
                foreach (var itemWord in itemWords)
                {
                    if (itemWord == themeWord) { DraftCategory = theme; return; }
                    if (themeWord.Length >= 4 && itemWord.Length >= 4 &&
                        (itemWord.StartsWith(themeWord, StringComparison.OrdinalIgnoreCase) ||
                         themeWord.StartsWith(itemWord, StringComparison.OrdinalIgnoreCase)))
                    {
                        DraftCategory = theme;
                        return;
                    }
                }
            }
        }
    }

    private static string[] Tokenize(string text)
    {
        if (string.IsNullOrWhiteSpace(text)) return Array.Empty<string>();
        return System.Text.RegularExpressions.Regex.Matches(text, @"[\p{L}\p{Nd}]{2,}")
            .Select(m => m.Value)
            .ToArray();
    }

    private void CreateList()
    {
        if (!CanCreate) return;

        var task = _controller.AddTask(DraftName.Trim(), DraftCategory.Trim());
        if (task is null) return;

        var taskIndex = _controller.TaskCount - 1;
        foreach (var draft in DraftItems)
            _controller.AddItemToTask(taskIndex, draft.Text);

        DraftName = string.Empty;
        DraftItemText = string.Empty;
        DraftItems.Clear();

        RefreshLists();
    }

    private void OnTasksChanged()
    {
        if (_disposed) return;
        Dispatcher.UIThread.Post(() =>
        {
            RefreshLists();
            RebuildAlerts();
        });
    }

    private void RefreshLists()
    {
        var query = SearchText.Trim();
        var today = DateOnly.FromDateTime(DateTime.Today);
        var filterIndex = DateFilterIndex;

        var vms = new List<TaskVm>();
        for (var i = _controller.TaskCount - 1; i >= 0; i--)
        {
            var task = _controller.GetTask(i);
            if (task is null) continue;
            if (query.Length > 0 && !task.Name.Contains(query, StringComparison.OrdinalIgnoreCase)) continue;
            if (!PassesDateFilter(task.Date, today, filterIndex)) continue;

            vms.Add(new TaskVm(_controller, i, onStateChanged: NotifyStateChanged, onCopyText: CopyItemText));
        }

        Groups.Clear();
        if (GroupModeIndex == 0)
            AddThemeGroups(vms);
        else
            AddDateGroups(vms, today);

        RefreshThemeOptions();
        Raise(nameof(HasLists));
        RebuildAlerts();
    }

    private void RefreshThemeOptions()
    {
        DraftThemeOptions.Clear();
        foreach (var category in _controller.Tasks
                     .Select(t => t.Category.Trim())
                     .Where(c => c.Length > 0)
                     .Distinct(StringComparer.OrdinalIgnoreCase)
                     .OrderBy(c => c, StringComparer.OrdinalIgnoreCase))
        {
            DraftThemeOptions.Add(category);
        }
    }

    private void AddDateGroups(List<TaskVm> vms, DateOnly today)
    {
        foreach (var label in DateGroupLabels(today))
        {
            var members = vms.Where(v => DateGroupLabel(v.Date, today) == label).ToList();
            if (members.Count == 0) continue;
            Groups.Add(new GroupVm(label, members));
        }
    }

    private void AddThemeGroups(List<TaskVm> vms)
    {
        var noTopic = vms.Where(v => !v.HasCategory).ToList();
        foreach (var category in vms.Where(v => v.HasCategory)
                                    .Select(v => v.Category)
                                    .Distinct(StringComparer.OrdinalIgnoreCase)
                                    .OrderBy(c => c, StringComparer.OrdinalIgnoreCase))
        {
            var members = vms.Where(v => string.Equals(v.Category, category, StringComparison.OrdinalIgnoreCase)).ToList();
            Groups.Add(new GroupVm(category, members));
        }

        if (noTopic.Count > 0)
            Groups.Add(new GroupVm("No theme", noTopic));
    }

    private static IReadOnlyList<string> DateGroupLabels(DateOnly today)
        => new[] { "Today", "Yesterday", "This week", "This month", "Earlier" };

    private static string DateGroupLabel(DateOnly d, DateOnly today)
    {
        if (d == today) return "Today";
        if (d == today.AddDays(-1)) return "Yesterday";
        if (d >= WeekStart(today)) return "This week";
        if (d.Year == today.Year && d.Month == today.Month) return "This month";
        return "Earlier";
    }

    private static DateOnly WeekStart(DateOnly today)
        => today.AddDays(-(((int)today.DayOfWeek + 6) % 7));

    private static bool PassesDateFilter(DateOnly date, DateOnly today, int index)
        => index switch
        {
            1 => date == today,
            2 => date >= WeekStart(today),
            3 => date.Year == today.Year && date.Month == today.Month,
            _ => true
        };

    private void NotifyStateChanged()
    {
        if (_disposed) return;
        Dispatcher.UIThread.Post(RebuildAlerts);
    }

    private void RebuildAlerts()
    {
        if (_disposed) return;

        var today = DateOnly.FromDateTime(DateTime.Today);
        var pending = new List<(Task task, int days)>();
        foreach (var task in _controller.Tasks)
        {
            if (task.IsFinished) continue;
            if (_dismissedAlertIds.Contains(task.Id)) continue;

            var days = today.DayNumber - task.Date.DayNumber;
            if (days < ReminderThresholdDays) continue;

            pending.Add((task, days));
        }
        pending.Sort((a, b) => b.days.CompareTo(a.days));

        var wasOpen = IsNotificationsOpen;
        Alerts.Clear();
        foreach (var (task, days) in pending)
        {
            Alerts.Add(new AlertVm(
                task.Id,
                task.Name,
                task.Category,
                "Created on " + Formatter.ShortDate(task.Date),
                "Pending for " + days + " days",
                DismissAlert));
        }

        if (!wasOpen && Alerts.Count == 0)
            IsNotificationsOpen = false;

        Raise(nameof(AlertCount));
        Raise(nameof(HasAlerts));
    }

    private void DismissAlert(AlertVm alert)
    {
        _dismissedAlertIds.Add(alert.TaskId);
        var item = Alerts.FirstOrDefault(a => a.TaskId == alert.TaskId);
        if (item is not null) Alerts.Remove(item);

        Raise(nameof(AlertCount));
        Raise(nameof(HasAlerts));
    }

    private void OnDraftItemsChanged(object? sender, NotifyCollectionChangedEventArgs e)
        => RaiseCanCreate();

    private void RaiseCanCreate()
    {
        Raise(nameof(CanCreate));
        Raise(nameof(HasDraftItems));
        CreateListCommand.RaiseCanExecuteChanged();
    }

    public void Dispose()
    {
        _disposed = true;
        _alertTimer.Stop();
        _controller.TasksChanged -= OnTasksChanged;
        DraftItems.CollectionChanged -= OnDraftItemsChanged;
        _controller.Dispose();
    }
}