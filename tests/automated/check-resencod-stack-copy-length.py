#!/usr/bin/env python3
"""Check compound-text stack copies cannot wrap at UINT_MAX."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib/Xm/ResEncod.c").read_text(
    encoding="utf-8"
)

assert source.count("if (len == UINT_MAX)") >= 2
assert source.count("XmStackAlloc(len + 1, tmp_buf)") >= 2
print("Compound-text stack-copy lengths are overflow-safe")
