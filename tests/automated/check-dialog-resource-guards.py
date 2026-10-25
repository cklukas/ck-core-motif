#!/usr/bin/env python3
"""Check DialogShell resource merging handles allocation failure."""

from pathlib import Path

source = Path(__file__).parents[2] / "lib" / "Xm" / "DialogS.c"
text = source.read_text()
start = text.index("static void \nClassInitialize")
end = text.index("/************************************************************************\n *\n *  ClassPartInit", start)
block = text[start:end]

if "SIZE_MAX / sizeof(XtResource)" not in block:
    raise SystemExit("resource merge size overflow is not guarded")
if "if (merged_list == NULL)" not in block:
    raise SystemExit("resource merge allocation is not guarded")
if block.count("class_initialize();") < 2:
    raise SystemExit("resource merge failure does not preserve class initialization")

print("DialogShell resource merging guards allocation failure")
