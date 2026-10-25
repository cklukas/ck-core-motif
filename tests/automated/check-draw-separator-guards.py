from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "Draw.c").read_text()
start = source.index("void XmeDrawSeparator(")
body = source[start:]

assert "line_span = (orientation == XmHORIZONTAL ? (int) width :" in body
assert "if (line_span <= 0)" in body
assert "if (!separator_gc)" in body
assert "if (!top_gc || !bottom_gc)" in body
