#!/usr/bin/env python3
"""Check null-client handling for MWM colormap reference updates."""

from pathlib import Path


source = Path("clients/mwm/WmColormap.c").read_text(encoding="utf-8")


def section(marker: str, end_marker: str) -> str:
    start = source.index(marker)
    return source[start : source.index(end_marker, start)]


for marker, end_marker in (
    ("void AddColormapWindowReference", "void RemoveColormapWindowReference"),
    ("void RemoveColormapWindowReference", "#endif"),
):
    body = section(marker, end_marker)
    assert "if (pCD == NULL)" in body
    assert "return;" in body
    assert body.index("if (pCD == NULL)") < body.index("XFindContext")

print("PASS MWM colormap reference updates reject null clients")
