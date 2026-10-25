#!/usr/bin/env python3
"""Check null-safe Xm cursor-size queries."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Screen.c").read_text(encoding="utf-8")
start = source.index("XmeQueryBestCursorSize(")
end = source.index("static XmConst char nullBits", start)
body = source[start:end]
assert "if (w == NULL || width == NULL || height == NULL)" in body
assert "if (xmScreen == NULL)" in body
assert body.count("_XmAppUnlock(app);") >= 2
print("Xm cursor-size queries validate widget and outputs")
