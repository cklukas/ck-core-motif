#!/usr/bin/env python3
"""Check DragUnder pixmap GC setup ignores unresolved depth metadata."""
from pathlib import Path

source = Path(__file__).resolve().parents[2] / "lib" / "Xm" / "DragUnder.c"
text = source.read_text(encoding="utf-8")
start = text.index("static void \nDrawHighlight(")
end = text.index("static void \nDrawShadow(", start)
highlight = text[start:end]
shadow_start = end
shadow_end = text.index("static void \n", shadow_start + 20)
shadow = text[shadow_start:shadow_end]
if highlight.count("depth = 0;") != 1 or highlight.count("else if (depth > 1)") != 1:
    raise SystemExit("DragUnder highlight pixmap depth guard missing")
if shadow.count("depth = 0;") != 2 or shadow.count("else if (depth > 1)") != 2:
    raise SystemExit("DragUnder shadow pixmap depth guards missing")
print("DragUnder pixmap depth guards are present")
