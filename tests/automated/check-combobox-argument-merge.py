#!/usr/bin/env python3
from pathlib import Path

source = Path("lib/Xm/ComboBox.c").read_text()
merges = source.count("XtMergeArgLists(")
guards = source.count("if (merged_args == NULL)")
assert merges >= 5
assert guards >= merges
print(f"ComboBox merged-argument guards present ({guards}/{merges})")
