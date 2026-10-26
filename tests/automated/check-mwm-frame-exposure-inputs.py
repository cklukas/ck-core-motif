#!/usr/bin/env python3
"""Check null-client handling in MWM frame exposure helpers."""

from pathlib import Path


source = Path("clients/mwm/WmCDecor.c").read_text(encoding="utf-8")


def body(marker: str, next_marker: str) -> str:
    start = source.index(marker)
    return source[start : source.index(next_marker, start)]


frame = body("Boolean FrameWindow (ClientData *pcd)", "void FrameExposureProc")
exposure = body("void FrameExposureProc (ClientData *pcd)", "void BaseWinExposureProc")
base = body("void BaseWinExposureProc (ClientData *pcd)", "Boolean ConstructFrame")

assert "if (pcd == NULL)" in frame
assert "return (FALSE);" in frame
assert frame.index("if (pcd == NULL)") < frame.index("ConstructFrame")

for function in (exposure, base):
    assert "if (pcd == NULL)" in function
    assert "return;" in function
    assert function.index("if (pcd == NULL)") < function.index("pcd->")

print("PASS MWM frame exposure helpers reject null clients")
