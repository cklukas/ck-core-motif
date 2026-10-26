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
assert body.count("if (requiredWidth > pcd->frameInfo.width)") >= 5
assert body.index("requiredWidth = 2UL") < body.index("*pWidth = (unsigned int)")
assert "FrameInteriorSpan (unsigned int total, unsigned int corner)" in source
helper_start = source.index("static int\nFrameInteriorSpan")
helper_end = source.index("Boolean GetFramePartInfo", helper_start)
helper = source[helper_start:helper_end]
assert "if (required > total)" in helper
assert "FrameInteriorSpan (pcd->frameInfo.width" in body
assert "FrameInteriorSpan (pcd->frameInfo.height" in body
assert body.count("pcd->frameInfo.lowerBorderWidth >") >= 2
assert body.count("*pWidth <= pcd->frameInfo.width") >= 2
assert "*pHeight <= pcd->frameInfo.height" in body
assert body.count("*pHeight <= pcd->frameInfo.height") >= 2
assert "FrameCornerSpan (unsigned int total, unsigned int corner)" in source
assert body.count("FrameCornerSpan (pcd->frameInfo.width") >= 4
assert body.count("FrameCornerSpan (pcd->frameInfo.height") >= 4

print("PASS MWM title geometry rejects impossible widths")
