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
        assert "natural_size(prompt" in text, name
        assert "MIN(USHRT_MAX" in text, name
    elif name == "Link.c":
        assert "natural_size(link" in text, name
        assert "MIN(USHRT_MAX" in text, name
    else:
        assert "natural_size(badge" in text, name
        assert "MIN(USHRT_MAX" in text, name
print("text-control preferred widths are bounded")
