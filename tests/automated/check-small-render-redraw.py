from pathlib import Path


for name in ("Avatar.c", "Badge.c", "Divider.c"):
    source = (Path(__file__).parents[2] / "lib" / "Xm" / name).read_text(
        encoding="utf-8"
    )
    start = source.index("static Boolean SetValues(Widget old_widget")
    body = source[start:source.index("static XtGeometryResult QueryGeometry", start)]
    assert "font_changed" in body
    assert "font_changed;" in body or "|| font_changed" in body
print("PASS Avatar, Badge, and Divider redraw on render-table changes")
