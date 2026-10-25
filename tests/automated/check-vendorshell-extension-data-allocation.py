#!/usr/bin/env python3
"""Check that VendorShell extension data is initialized transactionally."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "VendorSE.c").read_text()
start = source.index("extData = (XmWidgetExtData) XtCalloc")
end = source.index("\n\t  /*  Convert the fields", start)
region = source[start:end]

assert "if (extData == NULL)" in region
assert "extData->reqWidget = (Widget)" in region
assert "if (extData->reqWidget == NULL)" in region
assert "XtFree((char *)extData);" in region
assert region.index("_XmPushWidgetExtData") > region.index("memcpy( extData->reqWidget")

print("PASS VendorShell extension-data allocation is transactional")
