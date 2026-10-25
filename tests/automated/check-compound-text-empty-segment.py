#!/usr/bin/env python3
"""Check that compound-text output handles empty segments safely."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "ResEncod.c").read_text()
start = source.index("static void \noutputXmString")
end = source.index("static", start + 20)
body = source[start:end]

assert "if (ctx->itemlen == 0)" in body
assert "ctx->item[0]" in body
assert body.index("ctx->itemlen == 0") < body.index("ctx->item[0]")

print("compound-text empty-segment guard: ok")
