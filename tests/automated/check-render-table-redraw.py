from pathlib import Path


for name in ("DatePicker.c", "TimePicker.c", "IconLabel.c", "DataGrid.c"):
    source = (Path(__file__).parents[2] / "lib" / "Xm" / name).read_text(
        encoding="utf-8"
    )
    start = source.index("static Boolean SetValues(Widget old_widget")
    end_marker = "\nstatic void Redisplay" if "\nstatic void Redisplay" in source[start:] else "\nstatic XtGeometryResult QueryGeometry"
    body = source[start:source.index(end_marker, start)]
    assert "Boolean render_changed" in body
    assert "|| render_changed" in body
print("PASS render-table changes request redraws in all migrated widgets")
