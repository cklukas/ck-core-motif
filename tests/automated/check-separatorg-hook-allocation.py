#!/usr/bin/env python3
from pathlib import Path

source = Path("lib/Xm/SeparatoG.c").read_text()
start = source.rindex("SecondaryObjectCreate(")
end = source.index("SetValues(\n", start)
region = source[start:end]
assert region.count("if (newSec == NULL") >= 2
assert "if (newSec == NULL || reqSec == NULL)" in region
assert region.count("if (extData == NULL)") >= 3
assert region.count("if (ext == NULL)") >= 3
assert "_XmExtObjFree(newSec)" in region
assert "_XmExtObjFree((XtPointer)newSec)" in region
print("SeparatorGadget hook allocation guards present")
