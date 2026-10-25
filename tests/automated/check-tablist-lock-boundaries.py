#!/usr/bin/env python3
"""Check XmTabList does not call its locking free routine under the lock."""

from pathlib import Path


source = Path("lib/Xm/XmTabList.c").read_text()

assert "_XmProcessUnlock();\n\t    TabListFreeUnlocked(tl);" in source
assert "_XmProcessUnlock();\n  if (oldlist != NULL)\n    TabListFreeUnlocked(oldlist);" in source
assert source.count("XmTabListCopy(") == 1
assert source.count("XmTabListFree(") == 1
assert "TabListFreeUnlocked(oldlist);" in source

print("XmTabList free operations cross lock boundaries safely")
