#!/usr/bin/env python3
"""Check Manager class-extension allocations fail closed."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Manager.c").read_text()
start = source.index("static void \nClassPartInitialize(")
end = source.index("\n/************************************************************************", start)
region = source[start:end]

assert "comp_ext = XtNew(CompositeClassExtensionRec);" in region
assert "if (comp_ext == NULL)" in region
assert region.index("if (comp_ext == NULL)") < region.index("memcpy(comp_ext")
assert "*mext = (XmManagerClassExt) XtCalloc" in region
assert "if (*mext == NULL)" in region
assert region.index("if (*mext == NULL)") < region.index("(*mext)->record_type")

print("PASS Manager class-extension allocation guards present")
