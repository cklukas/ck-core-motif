#!/usr/bin/env python3
"""Check I18List GC creation tolerates render tables without XFontStructs."""
from pathlib import Path

source = Path("lib/Xm/I18List.c").read_text(encoding="utf-8")
assert "if (!XmeRenderTableGetDefaultFont" in source
assert "if (font)" in source
assert "mask |= GCFont" in source
assert "values.font = font->fid" in source
print("PASS I18List omits GCFont when the render table is Xft-only")
