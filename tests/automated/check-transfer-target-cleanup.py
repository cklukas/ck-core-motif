#!/usr/bin/env python3
"""Check that failed clipboard target conversions cannot double-free values."""

from pathlib import Path


source = Path("lib/Xm/Transfer.c").read_text(encoding="utf-8")

assert "for(i = 0; i < count; i++) {\n      name = GetSafeAtomName(display, targets[i], &howFree);\n      value = NULL;" in source
assert "XtFree((char*) value);\n      value = NULL;\n      FreeSafeAtomName" in source

print("clipboard target cleanup checks passed")
