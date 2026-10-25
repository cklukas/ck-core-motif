#!/usr/bin/env python3
"""Ensure Label accepts application-owned pixmaps outside the Motif cache."""

from pathlib import Path

source = (Path(__file__).resolve().parents[2] / "lib" / "Xm" / "Label.c").read_text()
start = source.index("LabelGetPixmapData(")
end = source.index("\n}\n", start)
region = source[start:end]

assert "XmeGetPixmapData" in region
assert "XGetGeometry" in region
assert "LabelGetPixmapData(XtScreen(newlw), Pix(newlw)" in source
assert "LabelGetPixmapData(XtScreen(lw), Pix(lw)" in source
print("PASS Label pixmap metadata fallback")
