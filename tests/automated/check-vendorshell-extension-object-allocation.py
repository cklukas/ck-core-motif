#!/usr/bin/env python3
from pathlib import Path

source = Path("lib/Xm/VendorS.c").read_text()
start = source.index("newSec = XtMalloc")
region = source[start:source.index("((XmVendorShellExtObject)newSec)->", start)]
assert "newSec = XtMalloc(size)" in region
assert "reqSec = _XmExtObjAlloc(size)" in region
assert "if (newSec == NULL || reqSec == NULL)" in region
assert "XtFree((char *)newSec)" in region
assert "_XmExtObjFree(reqSec)" in region
assert "if (extData == NULL)" in region
assert region.index("if (extData == NULL)") < region.index("extData->widget")
print("VendorShell extension-object allocation guards present")
