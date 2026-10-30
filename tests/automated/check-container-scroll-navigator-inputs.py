#!/usr/bin/env python3
"""Check Container scrolling validates navigator trait output."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Container.c").read_text(encoding="utf-8")
start = source.rindex("ScrollProc(")
region = source[start:source.index("\n}", start)]
assert region.index("if (num_nav_list != 0 && nav == NULL)") < region.index("for (i=0; i < num_nav_list")
assert region.index("if (*nav == NULL)") < region.index("XtClass(*nav)")
print("PASS Container scroll validates navigator trait output")
