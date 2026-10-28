#!/usr/bin/env python3
"""Check frame title layout validates the returned baseline count."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Frame.c").read_text()
start = source.index("static void \nCalcTitleExtent")
end = source.index("static void", start + len("static void"))
body = source[start:end]

assert "XmWidgetGetBaselines (fw->frame.title_area, &lines, &nlines)" in body
assert "nlines > 0 && lines != NULL" in body
assert "base = lines[nlines - 1];" in body

print("Frame title layout guards empty baseline results")
