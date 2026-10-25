#!/usr/bin/env python3
from pathlib import Path

source = Path("lib/Xm/LabelG.c").read_text()
start = source.rindex("GetValuesPrehook(Widget newParent")
end = source.index("SetValuesPosthook(Widget current", start)
region = source[start:end]
assert "newSec = (XmLabelGCacheObject)_XmExtObjAlloc(size);" in region
assert "if (newSec == NULL)" in region
assert "if (extData == NULL)" in region
assert "_XmExtObjFree((XtPointer)newSec)" in region
assert "GetValuesPosthook(Widget new_w" in region
assert "if (ext == NULL)" in region
print("LabelG get-values allocation guards present")
