#!/usr/bin/env python3
"""Check IconG RTL label placement avoids unsigned coordinate wraparound."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "IconG.c").read_text()

assert "label_x = (Position) ((int) XtWidth(wid) - (int) label_x -" in source
assert "(int) IG_LabelRectWidth(wid) - (int) IG_HLThickness(wid)" in source

print("IconG RTL label placement is signed")
