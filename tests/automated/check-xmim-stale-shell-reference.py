#!/usr/bin/env python3
"""Check that XmIm reconstruction tolerates released shell data."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "XmIm.c").read_text()
start = source.index("recreate_xic_info(XIC")
end = source.index("create_xic_info(Widget", start)
body = source[start:end]

assert "if (tmp_info == NULL)" in body
assert "assert(tmp_info != NULL)" not in body

print("XmIm stale shell-reference guard: ok")
