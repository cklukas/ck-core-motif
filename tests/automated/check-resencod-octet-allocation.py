#!/usr/bin/env python3
"""Check compound-text encoding concatenation guards its temporary buffer."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ResEncod.c").read_text()
start = source.index("cvtCTsegment(ct_context *ctx", 10000)
body = source[start:source.index("processExtendedSegments(", start)]
assert "octets =" in body
assert "if (octets == NULL)" in body
assert "return NULL;" in body
print("PASS compound-text encoding buffer allocation is guarded")
