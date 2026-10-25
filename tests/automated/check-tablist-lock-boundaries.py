#!/usr/bin/env python3
"""Check XmTabList free operations cross lock boundaries safely."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "XmTabList.c").read_text()

assert "static XmTabList TabListCopyUnlocked" in source
assert "static void TabListFreeUnlocked" in source
assert source.count("XmTabListCopy(") == 1
assert source.count("XmTabListFree(") == 1
assert "TabListFreeUnlocked(oldlist);" in source

print("XmTabList free operations cross lock boundaries safely")
