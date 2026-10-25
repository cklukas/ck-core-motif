#!/usr/bin/env python3
from pathlib import Path

source = Path("lib/Xm/LabelG.c").read_text()
start = source.index("SecondaryObjectCreate(Widget req")
end = source.index("memcpy(reqSec, newSec, size);", start)
region = source[start:end]
assert "newSec = _XmExtObjAlloc(size)" in region
assert "reqSec = _XmExtObjAlloc(size)" in region
assert "if (newSec == NULL || reqSec == NULL)" in region
assert "_XmExtObjFree(newSec)" in region
assert "_XmExtObjFree(reqSec)" in region
assert "if (extData == NULL)" in region
print("LabelG secondary-object allocation guards present")
