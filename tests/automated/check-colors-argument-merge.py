#!/usr/bin/env python3
from pathlib import Path

source = Path("lib/Xm/ColorS.c").read_text()
merges = source.count("XtMergeArgLists(")
guards = source.count("if (margs == NULL)")
assert merges >= 6
assert guards >= merges
print(f"ColorSelector merged-argument guards present ({guards}/{merges})")
