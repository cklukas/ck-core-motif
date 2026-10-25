#!/usr/bin/env python3
from pathlib import Path

source = Path("lib/Xm/PushBG.c").read_text()
start = source.rindex("SecondaryObjectCreate(")
end = source.index("SetValuesPosthook(", start)
region = source[start:end]
assert region.count("if (newSec == NULL") >= 2
assert "if (newSec == NULL || reqSec == NULL)" in region
assert region.count("if (extData == NULL)") >= 3
assert "_XmExtObjFree(newSec)" in region
assert "_XmExtObjFree((XtPointer)newSec)" in region
assert "if (ext == NULL)" in region
print("PushButtonGadget hook allocation guards present")
