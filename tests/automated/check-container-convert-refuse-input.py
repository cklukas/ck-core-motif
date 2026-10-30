#!/usr/bin/env python3
"""Check the Container refusal callback validates its callback payload."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Container.c").read_text(encoding="utf-8")
start = source.rindex("ConvertRefuse(")
region = source[start:source.index("\n}", start)]
guard = region.index("if (cs == NULL)")
assert guard < region.index("cs->value"), "refusal callback writes callback data before its null guard"
print("PASS Container refusal callback validates callback input")
