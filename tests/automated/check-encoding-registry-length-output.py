#!/usr/bin/env python3
"""Check encoding-registry conversion validates its length output pointer."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib/Xm/ResEncod.c").read_text()
start = source.index("_XmGetEncodingRegistryTarget(int *length)")
end = source.index("total_size = 0;", start)
body = source[start:end]

assert "if (length == NULL)" in body
assert "return NULL;" in body

print("PASS encoding-registry length output is validated")
