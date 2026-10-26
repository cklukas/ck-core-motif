from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "Prompt.c").read_text(
    encoding="utf-8"
)
start = source.index("static Boolean SetValues(Widget old_widget")
body = source[start:source.index("static XtGeometryResult QueryGeometry", start)]
assert "Boolean font_changed" in body
assert "return font_changed" in body
print("PASS Prompt redraws when its render table changes")
