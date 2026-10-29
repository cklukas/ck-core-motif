#!/usr/bin/env python3
"""Check TextOut visibility edges are compared in widened arithmetic."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "TextOut.c").read_text()
start = source.index("MakePositionVisible(")
start = source.index("MakePositionVisible(", start + 1)
end = source.index("static void \nBlinkInsertionPoint", start)
region = source[start:end]

assert region.count("(int64_t)tw->text.inner_widget->core.height -") >= 1
assert region.count("(int64_t)tw->text.inner_widget->core.width -") >= 1
assert "((Position) (tw->text.inner_widget->core.height -" not in region
assert "((Position) (tw->text.inner_widget->core.width -" not in region
print("PASS TextOut visibility edges use widened signed comparisons")
