#!/usr/bin/env python3
"""Check virtual-key binding-file buffer growth failure handling."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "VirtKeys.c").read_text()
start = source.index("_XmVirtKeysLoadFileBindings(")
end = source.index("\nstatic void \nLoadVendorBindings", start)
region = source[start:end]

assert "if (fileName == NULL || binding == NULL)" in region
assert "String new_binding;" in region
assert "new_binding = XtRealloc(*binding" in region
assert "if (new_binding == NULL)" in region
assert "String trimmed = XtRealloc(*binding, count + 1);" in region
assert "if (trimmed == NULL)" in region

print("PASS virtual-key binding allocation checks")
