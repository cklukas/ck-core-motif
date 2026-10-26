#!/usr/bin/env python3
"""Check that clipboard target and snapshot atom payloads are 32-bit data."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Transfer.c").read_text()
start = source.index("XmeClipboardSource(Widget w")
end = source.index("\nstatic void \nClipboardCallback", start)
region = source[start:end]
assert region.count("type == XA_ATOM && format == 32") >= 2
assert "size2 != 1 || format2 != 32" in region
print("clipboard target and snapshot conversions validate atom formats")
