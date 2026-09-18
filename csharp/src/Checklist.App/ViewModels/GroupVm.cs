using System.Collections.Generic;
using System.Collections.ObjectModel;

namespace Checklist.App.ViewModels;

/// <summary>Un grupo de listas ("My lists") con su encabezado y contador.</summary>
public sealed class GroupVm : ObservableObject
{
    public GroupVm(string name, IEnumerable<TaskVm> lists)
    {
        Name = name;
        Lists = new ObservableCollection<TaskVm>(lists);
    }

    public string Name { get; }

    public int Count => Lists.Count;

    public ObservableCollection<TaskVm> Lists { get; }
}