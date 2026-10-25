#!/usr/bin/env python3
from pathlib import Path

source = Path("lib/Xm/VendorS.c").read_text()
start = source.index("static void \nClassInitialize( void )")
end = source.index("_XmTransformSubResources", start)
region = source[start:end]
assert "size_t                      merged_count" in region
assert "SIZE_MAX / sizeof(XtResource)" in region
assert "UINT_MAX / sizeof(XtResource)" in region
assert "merged_list = (XtResource *)XtMalloc" in region
assert "if (merged_list == NULL)" in region
assert "_XmProcessUnlock();" in region
print("VendorShell resource-list allocation guard present")
