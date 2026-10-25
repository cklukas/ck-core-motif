#!/usr/bin/env python3
from pathlib import Path

source = Path("lib/Xm/List.c").read_text()
for marker in ("CopyItems(XmListWidget lw)",
               "CopySelectedItems(XmListWidget lw)",
               "BuildSelectedList(XmListWidget lw"):
    assert marker in source
assert source.count("SIZE_MAX / sizeof(XmString)") >= 3
assert source.count("UINT_MAX / sizeof(XmString)") >= 3

print("List item-copy allocations honor Xt Cardinal byte limits")
