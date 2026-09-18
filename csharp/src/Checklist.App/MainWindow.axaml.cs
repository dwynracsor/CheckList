using System.ComponentModel;
using Avalonia;
using Avalonia.Controls;
using Avalonia.Input;
using Avalonia.Markup.Xaml;
using Avalonia.Styling;
using Checklist.App.ViewModels;

namespace Checklist.App;

public partial class MainWindow : Window
{
    private MainViewModel? _vm;

    public MainWindow()
    {
        InitializeComponent();
        DataContextChanged += (_, _) => HookViewModel();
        HookViewModel();
        KeyDown += OnWindowKeyDown;
    }

    /// <summary>
    /// Escucha IsDarkMode del view model para alternar el ThemeVariant global,
    /// que es lo que hace que los ThemeDictionaries de Theme.axaml cambien.
    /// </summary>
    private void HookViewModel()
    {
        if (_vm is not null)
            _vm.PropertyChanged -= OnViewModelPropertyChanged;

        _vm = DataContext as MainViewModel;

        if (_vm is not null)
        {
            _vm.PropertyChanged += OnViewModelPropertyChanged;
            _vm.AttachClipboard(Clipboard);
            ApplyTheme();
        }
    }

    private void OnViewModelPropertyChanged(object? sender, PropertyChangedEventArgs e)
    {
        if (e.PropertyName == nameof(MainViewModel.IsDarkMode))
            ApplyTheme();
    }

    private void ApplyTheme()
    {
        if (_vm is null) return;

        var variant = _vm.IsDarkMode ? ThemeVariant.Dark : ThemeVariant.Light;

        RequestedThemeVariant = variant;
        if (Application.Current is not null)
            Application.Current.RequestedThemeVariant = variant;
    }

    // --- Shortcuts ---

    private void OnWindowKeyDown(object? sender, KeyEventArgs e)
    {
        if (_vm is null) return;
        if (!e.KeyModifiers.HasFlag(KeyModifiers.Control)) return;

        switch (e.Key)
        {
            case Key.N:
                _vm.IsCreateExpanded = !_vm.IsCreateExpanded;
                e.Handled = true;
                break;
            case Key.S:
                _vm.Controller.Save();
                e.Handled = true;
                break;
            case Key.Q:
                Close();
                e.Handled = true;
                break;
            case Key.Z:
                _vm.Controller.Undo();
                e.Handled = true;
                break;
            case Key.Y:
                _vm.Controller.Redo();
                e.Handled = true;
                break;
        }
    }
}