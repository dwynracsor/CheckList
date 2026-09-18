using System;
using System.Windows.Input;

namespace Checklist.App.ViewModels;

/// <summary>Una alerta mostrada en el panel de notificaciones (lista pendiente por tiempo).</summary>
public sealed class AlertVm : ObservableObject
{
    private readonly Action<AlertVm> _onDismiss;

    public AlertVm(
        string taskId,
        string listName,
        string category,
        string createdText,
        string pendingText,
        Action<AlertVm> onDismiss)
    {
        TaskId = taskId;
        ListName = listName;
        Category = category;
        CreatedText = createdText;
        PendingText = pendingText;
        _onDismiss = onDismiss;

        DismissCommand = new RelayCommand(_ => _onDismiss(this));
    }

    public string TaskId { get; }

    public string ListName { get; }

    public string Category { get; }

    public string CreatedText { get; }

    public string PendingText { get; }

    public ICommand DismissCommand { get; }
}