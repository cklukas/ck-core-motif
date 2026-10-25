#!/usr/bin/env python3
"""Check TextField selection exports validate legacy int spans."""
from pathlib import Path

source = Path("lib/Xm/TextFSel.c").read_text(encoding="utf-8")
assert "text_field_selection_span_to_int" in source
assert "length > (XmTextPosition)INT_MAX" in source
assert source.count("text_field_selection_span_to_int(left, right, &selection_span)") == 4
assert "(int)(right - left)" not in source
print("PASS TextField selection exports validate position spans before narrowing")
