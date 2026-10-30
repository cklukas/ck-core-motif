#!/usr/bin/env python3
"""Check Container marquee drawing validates its widget input."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Container.c").read_text(encoding="utf-8")
start = source.rindex("DrawMarquee(\n")
region = source[start:source.index("\n}", start)]
assert region.index("if (wid == NULL)") < region.index("XtIsRealized(wid)")
print("PASS Container marquee drawing validates widget input")
