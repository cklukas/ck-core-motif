#!/usr/bin/env python3
"""Check IconG RTL label placement avoids unsigned coordinate wraparound."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "IconG.c").read_text()

assert "label_x = bounded_position((long long)XtWidth(wid) - label_x -" in source
assert "IG_LabelRectWidth(wid) - IG_HLThickness(wid));" in source

print("IconG RTL label placement is signed")
