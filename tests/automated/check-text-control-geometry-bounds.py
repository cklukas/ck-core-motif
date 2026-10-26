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
    assert "(Dimension)-1" in body, name
    if name == "Prompt.c":
        assert "width > (Dimension)-1 - 8" in body, name
        assert "height > (Dimension)-1 - 8" in body, name
    elif name == "Link.c":
        assert "(unsigned long)(Dimension)-1 - 8UL" in body, name
    else:
        assert "size_t length;" in body, name
        assert f"- {padding}" in body, name
print("text-control preferred widths are bounded")
