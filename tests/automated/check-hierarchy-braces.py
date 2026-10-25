#!/usr/bin/env python3
"""Check that hierarchy child insertion closes its outer search block."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Xm" / "Hierarchy.c"
text = source.read_text(encoding="utf-8")
assert "\t\tbreak;\n\t    }\n    }\n    }\n    \n    if (success == NO)" in text
print("hierarchy child insertion control blocks are balanced")
