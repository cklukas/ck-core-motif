#!/usr/bin/env python3
"""Check null-client handling in MWM base-window metrics."""

from pathlib import Path


source = Path("clients/mwm/WmCDInfo.c").read_text(encoding="utf-8")


def body(marker: str, next_marker: str) -> str:
    start = source.index(marker)
    return source[start : source.index(next_marker, start)]


width = body("unsigned int BaseWindowWidth (ClientData *pcd)", "unsigned int BaseWindowHeight")
height = body("unsigned int BaseWindowHeight (ClientData *pcd)", "static int\nFrameInteriorSpan")
for function in (width, height):
    assert "if (pcd == NULL)" in function
    assert "return (0);" in function
    assert function.index("if (pcd == NULL)") < function.index("pcd->")

print("PASS MWM base-window metrics reject null clients")
