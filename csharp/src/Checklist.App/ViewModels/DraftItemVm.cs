using System;
using System.Windows.Input;

namespace Checklist.App.ViewModels;

/// <summary>
/// Item del bloque "Create list": se acumula en la vista mientras se arma la
/// lista, antes de que exista en el controlador.
/// </summary>
public sealed class DraftItemVm : ObservableObject
{
    private readonly Action<DraftItemVm> _onDelete;
    private string _text;

    public DraftItemVm(string text, Action<DraftItemVm> onDelete)
    {
        _text = text;
        _onDelete = onDelete;
        DeleteCommand = new RelayCommand(_ => _onDelete(this));
    }

    public string Text
    {
        get => _text;
        set => SetProperty(ref _text, value);
    }

    public ICommand DeleteCommand { get; }
}