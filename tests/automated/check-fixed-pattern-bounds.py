#!/usr/bin/env python3
"""Check Xmos fixed-pattern conversion respects Xt allocation widths."""

from pathlib import Path


source = Path("lib/Xm/Xmos.c").read_text()

assert "pattern_len > (size_t)UINT_MAX - 4" in source
assert "XtCalloc(2, (Cardinal)(pattern_len + 4))" in source

print("fixed-pattern allocation width is guarded")
