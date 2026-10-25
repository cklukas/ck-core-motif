#!/usr/bin/env python3
"""Check right-to-left TabBox placement avoids unsigned wraparound."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "TabBox.c").read_text()

assert "(int) tab->core.width - (int) wanted[0].width" in source

print("TabBox right-to-left placement arithmetic is signed")
