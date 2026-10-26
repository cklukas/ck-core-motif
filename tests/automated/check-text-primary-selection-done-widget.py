#!/usr/bin/env python3
"""Check Text primary-selection completion callbacks validate widgets."""

from pathlib import Path


for name in ("TextSel.c", "TextFSel.c"):
    source = (Path(__file__).parents[2] / "lib" / "Xm" / name).read_text()
    start = source.rindex("SetPrimarySelection(Widget w")
    end = source.find("\n\n/*", start)
    body = source[start:] if end == -1 else source[start:end]
    assert "if (w == NULL)" in body, name
    assert "(XmText" in body, name

print("Text primary-selection completion callbacks validate widgets")
