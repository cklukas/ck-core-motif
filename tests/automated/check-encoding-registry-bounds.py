#!/usr/bin/env python3
"""Check checked sizing for the compound-text encoding registry target."""
from pathlib import Path

source = (Path(__file__).resolve().parents[2] / "lib/Xm/ResEncod.c").read_text()
start = source.index("_XmGetEncodingRegistryTarget(int *length)")
end = source.index("\n}\n", start) + 3
body = source[start:end]
assert "tag_size > SIZE_MAX - encoding_size - 2" in body
assert "total_size + tag_size + encoding_size + 2 > (size_t)INT_MAX" in body
assert "if (!total_size)" in body
assert "XtMalloc((Cardinal)total_size)" in body
print("encoding-registry sizing contract passed")
