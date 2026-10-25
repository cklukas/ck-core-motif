#!/usr/bin/env python3
"""Check null-screen handling in XmeGetPixmapData."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "GetPixData.c").read_text(encoding="utf-8")
start = source.index("XmeGetPixmapData(")
end = source.index("\n}\n", start) + 3
body = source[start:end]
assert "if (screen == NULL)" in body
assert "return False;" in body
assert body.index("if (screen == NULL)") < body.index("DisplayOfScreen(screen)")
print("Xme pixmap-data queries validate their screen")
