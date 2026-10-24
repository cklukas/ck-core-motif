#!/usr/bin/env python3
"""Check that scaled-pixmap screen fallback is null-safe."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ImageCache.c").read_text()
start = source.index("_XmGetScaledPixmap(")
end = source.index("\n\n\n/*******************************************************************", start)
region = source[start:end]
assert "if (!screen && widget) screen = XtScreen(widget);" in region
assert "if (!screen)" in region
assert "display = DisplayOfScreen(screen);" in region
assert region.index("if (!screen && widget)") < region.index("DisplayOfScreen(screen)")
print("scaled-pixmap screen fallback is null-safe")
