#!/usr/bin/env python3
"""Check null and overflow-safe handling in MWM corner metrics."""

from pathlib import Path


source = Path("clients/mwm/WmCDInfo.c").read_text(encoding="utf-8")


def body(marker: str, next_marker: str) -> str:
    start = source.index(marker)
    return source[start : source.index(next_marker, start)]


width = body("unsigned int CornerWidth (ClientData *pcd)", "unsigned int FrameHeight")
height = body("unsigned int CornerHeight (ClientData *pcd)", "int BaseWindowX")
for function in (width, height):
    assert "if (pcd == NULL)" in function
    assert "return (0);" in function
    assert function.index("if (pcd == NULL)") < function.index("pcd->decor")

assert "3UL*cwidth > frameWidth" in width
assert "3UL*cheight > frameHeight" in height

print("PASS MWM corner metrics reject null clients and avoid narrow products")
