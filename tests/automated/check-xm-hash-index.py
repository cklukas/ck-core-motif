#!/usr/bin/env python3
"""Check Xm hash values are normalized before bucket indexing."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Hash.c").read_text()
helper = source[source.index("HashIndex(XmHashValue"):source.index("/* Available table sizes")]
assert "(unsigned long)hash % size" in helper
body = source[source.index("_XmResizeHashTable"):source.index("/* Bucket management */")]
assert body.count("HashIndex(") >= 5
assert "% table -> size" not in body
print("PASS Xm hash indices normalize signed callback values")
