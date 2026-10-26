#!/usr/bin/env python3
"""Check that both compound-text CSI decoders validate lookahead bytes."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Xm" / "ResEncod.c"
text = source.read_text(encoding="utf-8")
check = "if ((ctx->lastoctet - ctx->octet) < 3)"
if text.count(check) < 2:
    raise SystemExit("compound-text CSI lookahead bounds are incomplete")

print("compound-text CSI lookahead is bounded")
