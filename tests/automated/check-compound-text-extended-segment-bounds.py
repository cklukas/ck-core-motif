#!/usr/bin/env python3
"""Check bounds on the compatibility extended-segment separator scan."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "ResEncod.c").read_text()
start = source.index("processExtendedSegmentsHack(\n        ct_context")
end = source.index("static XmString", start)
body = source[start:end]

assert "while (len < seglen && esptr[len] != STX)" in body
assert "if (len >= seglen)" in body
assert "if ((ctx->lastoctet - ctx->octet) < 2)" in body
assert "while (esptr[len] != STX)" not in body

print("compound-text extended-segment bounds: ok")
