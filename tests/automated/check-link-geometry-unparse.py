from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "Link.c").read_text(
    encoding="utf-8"
)
geometry = source[source.index("static XtGeometryResult QueryGeometry") :]
assert "char text[256]" not in geometry
assert "text_value(link" not in geometry
assert "XmStringExtent(link->link.render_table" in geometry
print("PASS Link geometry uses the full XmString without fixed-buffer unparse")
