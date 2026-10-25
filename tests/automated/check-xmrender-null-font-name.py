#!/usr/bin/env python3
"""Check rendition cleanup tolerates an omitted font name."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "XmRenderT.c").read_text()
start = source.index("static void\nCleanupResources(")
end = source.index("\n\n/* Emit warning", start)
region = source[start:end]

assert "_XmRendFontName(rend) == NULL" in region
assert "strcmp(_XmRendFontName(rend), XmSXmAS_IS)" in region
print("PASS CleanupResources guards a missing font name before strcmp")
