#!/usr/bin/env python3
"""Check title geometry rejects widths that would underflow."""

from pathlib import Path


source = Path("clients/mwm/WmCDInfo.c").read_text(encoding="utf-8")
start = source.index(
    "Boolean GetFramePartInfo (ClientData *pcd, int part, int *pX, int *pY,"
)
end = source.index("/*************************************<->", start + 1)
body = source[start:end]

assert "requiredWidth = 2UL * pcd->frameInfo.upperBorderWidth;" in body
assert "if (requiredWidth > pcd->frameInfo.width)" in body
assert "*pWidth = (unsigned int)(pcd->frameInfo.width - requiredWidth);" in body
assert body.index("requiredWidth = 2UL") < body.index("*pWidth = (unsigned int)")

print("PASS MWM title geometry rejects impossible widths")
