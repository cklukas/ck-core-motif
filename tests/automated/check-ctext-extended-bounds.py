#!/usr/bin/env python3
"""Check that both extended compound-text decoders bound segment reads."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Xm" / "ResEncod.c"
text = source.read_text(encoding="utf-8")
check = "if ((ctx->lastoctet - ctx->octet) < seglen)"
if text.count(check) < 2:
    raise SystemExit("extended compound-text segment bounds are incomplete")

print("extended compound-text segment reads are bounded")
