#!/usr/bin/env python3
"""Check that virtual-key vendor matching handles its formatting buffer."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "VirtKeys.c").read_text(encoding="utf-8")
start = source.index("LoadVendorBindings(Display")
end = source.index("int \n_XmVirtKeysLoadFallbackBindings", start)
region = source[start:end]
assert "if (vendor == NULL)" in region
assert "vendor_length = strlen(vendor);" in region
assert "vendor_length > (size_t)UINT_MAX - 32" in region
assert "vendor_capacity = vendor_length + 32;" in region
assert "vendorV = XtMalloc ((Cardinal)vendor_capacity);" in region
assert "if (vendorV == NULL)" in region
assert "vendor_written = snprintf" in region
assert "(size_t)vendor_written >= vendor_capacity" in region
print("PASS virtual-key vendor formatting allocation is guarded")
