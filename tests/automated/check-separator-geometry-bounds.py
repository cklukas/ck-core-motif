#!/usr/bin/env python3
"""Check XmSeparator spans use saturating geometry arithmetic."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Separator.c").read_text()

assert "static Dimension separator_span(Dimension highlight, Dimension extra);" in source
assert source.count("separator_span(new_w") >= 12
assert "uint64_t total = 2 * (uint64_t)highlight" in source
assert "total > USHRT_MAX" in source
assert "core.height +=" not in source
assert "core.width +=" not in source
print("PASS Separator spans saturate")
