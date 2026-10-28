#!/usr/bin/env python3
"""Check failed file binding loads do not publish partial strings."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "VirtKeys.c").read_text()
start = source.index("_XmVirtKeysLoadFileBindings(")
end = source.index("static void \nLoadVendorBindings", start)
body = source[start:end]

assert "if (!complete)" in body
assert "XtFree(*binding);" in body
assert "*binding = NULL;" in body
assert "if (trimmed == NULL)" in body

print("Failed virtual-key file loads discard partial bindings")
