#!/usr/bin/env python3
"""Check null-client handling in MWM title metrics."""

from pathlib import Path


source = Path("clients/mwm/WmCDInfo.c").read_text(encoding="utf-8")


def body(marker: str, next_marker: str) -> str:
    start = source.index(marker)
    return source[start : source.index(next_marker, start)]


text = body("unsigned int TitleTextHeight (ClientData *pcd)", "unsigned int InitTitleBarHeight")
bar = body("unsigned int InitTitleBarHeight (ClientData *pcd)", "unsigned int UpperBorderWidth")
for function in (text, bar):
    assert "if (pcd == NULL)" in function
    assert "return (0);" in function
    assert function.index("if (pcd == NULL)") < function.index("DECOUPLE_TITLE_APPEARANCE") if "DECOUPLE_TITLE_APPEARANCE" in function else function.index("pcd->decor")

print("PASS MWM title metrics reject null clients")
