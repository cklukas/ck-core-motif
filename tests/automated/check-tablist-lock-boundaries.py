#!/usr/bin/env python3
"""Check XmTabList does not call its locking free routine under the lock."""

from pathlib import Path


source = Path("lib/Xm/XmTabList.c").read_text()

assert "_XmProcessUnlock();\n\t    XmTabListFree(tl);" in source
assert "_XmProcessUnlock();\n  if (oldlist != NULL)\n    XmTabListFree(oldlist);" in source

print("XmTabList free operations cross lock boundaries safely")
