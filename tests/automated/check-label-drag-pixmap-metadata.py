#!/usr/bin/env python3
"""Check Label and LabelG drag-icon pixmap metadata outputs are initialized."""
from pathlib import Path

root = Path(__file__).resolve().parents[2] / "lib" / "Xm"
checks = {
    "Label.c": ("unsigned int width, height;", "width = height = 0;", "depth = 0;"),
    "LabelG.c": ("unsigned int wid, hei;", "wid = hei = 0;", "d = 0;"),
}
for filename, (declaration, width_reset, depth_reset) in checks.items():
    text = (root / filename).read_text(encoding="utf-8")
    start = text.rindex("GetPixmapDragIcon(Widget w)")
    end = text.index("\f", start)
    body = text[start:end]
    for fragment in (declaration, width_reset, depth_reset, "XmeGetPixmapData"):
        if fragment not in body:
            raise SystemExit(f"{filename} drag-icon metadata guard missing: {fragment}")
print("Label and LabelG drag-icon pixmap metadata outputs are initialized")
