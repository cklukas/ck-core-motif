#!/usr/bin/env python3
"""Check minimum-length guards in compound-text dispatchers."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "ResEncod.c").read_text()
for name in ("processESCHack", "processESC", "processCSI"):
    start = source.index(name + "(\n        ct_context")
    start = source.index("{", start)
    end = source.index("\n}", start)
    body = source[start:end]
    assert "if (ctx->itemlen < 2)" in body
    assert body.index("ctx->itemlen < 2") < body.index("ctx->item[1]")

print("compound-text control-sequence bounds: ok")
