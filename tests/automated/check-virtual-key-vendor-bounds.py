#!/usr/bin/env python3
"""Check virtual-key vendor formatting bounds its Xt allocation."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "VirtKeys.c").read_text()
start = source.index("static void \nLoadVendorBindings")
body = source[start:source.index("int \n_XmVirtKeysLoadFallbackBindings", start)]

assert "vendor_length = strlen(vendor);" in body
assert "vendor_length > (size_t)UINT_MAX - 32" in body
assert "vendorV = XtMalloc ((Cardinal)vendor_capacity);" in body
assert "snprintf(vendorV, vendor_capacity" in body
assert "sprintf (vendorV" not in body
print("PASS virtual-key vendor formatting bounds allocation and output")
