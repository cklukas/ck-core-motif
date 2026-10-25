#!/usr/bin/env python3
"""Check that virtual-key vendor matching handles its formatting buffer."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "VirtKeys.c").read_text(encoding="utf-8")
start = source.index("LoadVendorBindings(Display")
end = source.index("int \n_XmVirtKeysLoadFallbackBindings", start)
region = source[start:end]
assert "if (vendor == NULL)" in region
assert "vendorV = XtMalloc (strlen(vendor) + 20);" in region
assert "if (vendorV == NULL)" in region
assert "sprintf (vendorV" in region
print("PASS virtual-key vendor formatting allocation is guarded")
