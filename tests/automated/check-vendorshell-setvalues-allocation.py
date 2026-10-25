#!/usr/bin/env python3
"""Check that VendorShell SetValues snapshots are allocated before publish."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "VendorS.c").read_text()
start = source.index("SetValuesPrehook(")
start = source.index("    oldExtData =", start)
end = source.index("\n    /*  Convert the necessary fields", start)
region = source[start:end]

assert "newExtData->oldWidget = (Widget) _XmExtObjAlloc" in region
assert "newExtData->reqWidget = (Widget) _XmExtObjAlloc" in region
assert "if (newExtData->oldWidget == NULL || newExtData->reqWidget == NULL)" in region
assert "_XmExtObjFree((XtPointer)newExtData->oldWidget);" in region
assert "_XmExtObjFree((XtPointer)newExtData->reqWidget);" in region
assert region.index("_XmPushWidgetExtData") > region.index("memcpy((char *)newExtData->reqWidget")

print("PASS VendorShell SetValues snapshots are transactional")
