using System;

namespace Checklist.App.ViewModels;

/// <summary>Formato de fechas: día, mes abreviado en mayúsculas y año (ej. "17 SEP 2026").</summary>
public static class Formatter
{
    private static readonly string[] Months =
    {
        "", "ENE", "FEB", "MAR", "ABR", "MAY", "JUN",
        "JUL", "AGO", "SEP", "OCT", "NOV", "DIC"
    };

    public static string ShortDate(DateOnly date)
        => $"{date.Day:D2} {Months[date.Month]} {date.Year}";
}