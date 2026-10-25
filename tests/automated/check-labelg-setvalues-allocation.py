#!/usr/bin/env python3
from pathlib import Path

source = Path("lib/Xm/LabelG.c").read_text()
start = source.rindex("SetValuesPrehook(Widget oldParent")
end = source.index("GetValuesPrehook(Widget newParent", start)
region = source[start:end]
assert "newSec = (XmLabelGCacheObject)_XmExtObjAlloc(size);" in region
assert "reqSec = (XmLabelGCacheObject)_XmExtObjAlloc(size);" in region
assert "if (newSec == NULL || reqSec == NULL)" in region
assert "_XmExtObjFree((XtPointer)newSec)" in region
assert "_XmExtObjFree((XtPointer)reqSec)" in region
assert "if (extData == NULL)" in region
print("LabelG set-values allocation guards present")
