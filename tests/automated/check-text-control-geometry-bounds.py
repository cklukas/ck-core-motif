#!/usr/bin/env python3
"""Check text-control preferred widths avoid Dimension overflow."""

from pathlib import Path

root = Path(__file__).parents[2] / "lib/Xm"
for name, padding in (("Prompt.c", "8UL"), ("Link.c", "8UL"), ("Badge.c", "12UL")):
    text = (root / name).read_text(encoding="utf-8")
    start = text.index("static XtGeometryResult QueryGeometry")
    tail = text[start:]
    end = tail.index("Widget XmCreate") if "Widget XmCreate" in tail else tail.index("void Xm")
    body = tail[:end]
    if name == "Prompt.c":
        assert "padded_extent(width, 8)" in body, name
        assert "padded_extent(height, 8)" in body, name
    elif name == "Link.c":
        assert "(Dimension)-1" in body, name
        assert "(unsigned long)(Dimension)-1 - 8UL" in body, name
    else:
        assert "(Dimension)-1" in body, name
        assert "size_t length;" in body, name
        assert f"- {padding}" in body, name
print("text-control preferred widths are bounded")
