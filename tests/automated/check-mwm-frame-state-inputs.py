#!/usr/bin/env python3
"""Check null-client handling in MWM frame-state helpers."""

from pathlib import Path


source = Path("clients/mwm/WmCDInfo.c").read_text(encoding="utf-8")


def body(marker: str, next_marker: str) -> str:
    start = source.index(marker)
    return source[start : source.index(next_marker, start)]


set_info = body("void SetFrameInfo (ClientData *pcd)", "void SetClientOffset")
set_offset = body("void SetClientOffset (ClientData *pcd)", "Boolean XBorderIsShowing")
border_start = source.index("Boolean XBorderIsShowing (ClientData *pcd)")
border = source[border_start:]

for function in (set_info, set_offset):
    assert "if (pcd == NULL)" in function
    assert "return;" in function
    assert function.index("if (pcd == NULL)") < function.index("pcd->")

assert "if (pcd == NULL)" in border
assert "return (False);" in border
assert border.index("if (pcd == NULL)") < border.index("pcd->")

print("PASS MWM frame-state helpers reject null clients")
