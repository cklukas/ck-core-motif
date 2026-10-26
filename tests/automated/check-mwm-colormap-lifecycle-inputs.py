#!/usr/bin/env python3
"""Check invalid-input guards in MWM colormap lifecycle helpers."""

from pathlib import Path


source = Path("clients/mwm/WmColormap.c").read_text(encoding="utf-8")


def body(marker: str, end_marker: str) -> str:
    start = source.index(marker)
    return source[start : source.index(end_marker, start)]


for marker, end_marker in (
    ("void InitWorkspaceColormap", "void InitColormapFocus"),
    ("void InitColormapFocus", "#ifndef OLD_COLORMAP"),
    ("void ForceColormapFocus", "#endif"),
    ("void SetColormapFocus", "void WmInstallColormap"),
    ("void WmInstallColormap", "void ResetColormapData"),
    ("void ResetColormapData", "#ifndef IBM_169380"),
    ("ProcessColormapList (WmScreenData *pSD, ClientData *pCD)",
     "static Bool\nProcessEvents"),
):
    section = body(marker, end_marker)
    assert "pSD == NULL" in section or "pCD == NULL" in section
    assert "return" in section

reset = body("void ResetColormapData", "#ifndef IBM_169380")
assert "count < 0" in reset
assert "count > 0 && pWindows == NULL" in reset

print("PASS MWM colormap lifecycle helpers reject invalid inputs")
