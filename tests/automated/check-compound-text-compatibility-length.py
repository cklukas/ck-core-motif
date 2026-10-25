#!/usr/bin/env python3
"""Check bounds initialization for the legacy compound-text entry point."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "ResEncod.c").read_text()
start = source.index("XmCvtCTToXmString(\n        char *text )")
end = source.index("static Boolean", start)
body = source[start:end]

assert "if (text == NULL)" in body
assert "ctx->lastoctet = ctx->octet + strlen(text);" in body
assert "((ctx->lastoctet - ctx->octet) >= 4)" in body

print("compound-text compatibility length: ok")
