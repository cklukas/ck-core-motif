#!/usr/bin/env python3
"""Check null-client handling in MWM frame-coordinate metrics."""

from pathlib import Path


source = Path("clients/mwm/WmCDInfo.c").read_text(encoding="utf-8")


def body(marker: str, next_marker: str) -> str:
    start = source.index(marker)
    return source[start : source.index(next_marker, start)]


functions = (
    body("int BaseWindowX (ClientData *pcd)", "int BaseWindowY"),
    body("int BaseWindowY (ClientData *pcd)", "int FrameX"),
    body("int FrameX (ClientData *pcd)", "int FrameY"),
    body("int FrameY (ClientData *pcd)", "unsigned int BaseWindowWidth"),
)
for function in functions:
    assert "if (pcd == NULL)" in function
    assert "return (0);" in function
    assert function.index("if (pcd == NULL)") < function.index("pcd->")

print("PASS MWM frame-coordinate metrics reject null clients")
