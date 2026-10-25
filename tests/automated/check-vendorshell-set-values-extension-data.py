#!/usr/bin/env python3
from pathlib import Path

source = Path("lib/Xm/VendorS.c").read_text()
start = source.rindex("newExtData = (XmWidgetExtData) XtCalloc")
end = source.index("newExtData->widget", start)
region = source[start:end]
assert "if (newExtData == NULL)" in region
assert "return;" in region
print("VendorShell set-values extension-data guard present")
