#!/usr/bin/env python3
"""Check MakePositionVisible keeps target-position arithmetic inside its guards."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib/Xm/TextOut.c").read_text(
    encoding="utf-8"
)
start = source.index("\nMakePositionVisible(XmTextWidget")
body = source[start:source.index("static void", start + 20)]
assert body.count("target_position = position >") == 2
assert body.count("position = target_position > line_start") == 2
assert body.count("{\n\t    line_start") >= 1
assert body.count("{\n\t  line_start") >= 1
print("PASS MakePositionVisible only adjusts positions at line boundaries")
