#!/usr/bin/env python3
"""Check null-client handling in MWM border metrics."""

from pathlib import Path


source = Path("clients/mwm/WmCDInfo.c").read_text(encoding="utf-8")


def body(marker: str, next_marker: str) -> str:
    start = source.index(marker)
    return source[start : source.index(next_marker, start)]


upper = body("unsigned int UpperBorderWidth (ClientData *pcd)", "unsigned int LowerBorderWidth")
lower = body("unsigned int LowerBorderWidth (ClientData *pcd)", "unsigned int FrameWidth")
for function in (upper, lower):
    assert "if (pcd == NULL)" in function
    assert "return (0);" in function
    assert function.index("if (pcd == NULL)") < function.index("pcd->")

print("PASS MWM border metrics reject null clients")
