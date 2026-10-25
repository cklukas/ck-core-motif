#!/usr/bin/env python3
"""Check List geometry calculations use signed intermediates before clamping."""
from pathlib import Path

source = Path(__file__).resolve().parents[2] / "lib" / "Xm" / "List.c"
text = source.read_text(encoding="utf-8")
start = text.index("static void\nDrawHighlight(")
end = text.index("static void\nSetClipRect", start)
highlight = text[start:end]
if "int        width, height;" not in highlight:
    raise SystemExit("List highlight dimensions are not signed")
if "width = (int)lw->core.width -" not in highlight:
    raise SystemExit("List highlight width is not signed before subtraction")
if "height = (int)lw->core.height -" not in highlight:
    raise SystemExit("List highlight height is not signed before subtraction")
scroll = text[text.index("SetHorizontalScrollbar("):]
if "listwidth = (int)lw->core.width -" not in scroll:
    raise SystemExit("List horizontal scrollbar width is not signed")
print("List geometry calculations use signed intermediates")
