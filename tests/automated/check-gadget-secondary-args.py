#!/usr/bin/env python3
from pathlib import Path

source = Path("lib/Xm/Gadget.c").read_text()
start = source.index("SecondaryObjectCreate(")
end = source.index("Initialize(\n", start)
region = source[start:end]
assert "mergedArgs = XtMergeArgLists" in region
assert "if (mergedArgs == NULL)" in region
assert "return;" in region
assert "XtCreateWidget" in region
print("Gadget secondary-argument allocation guard present")
