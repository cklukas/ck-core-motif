#!/usr/bin/env python3
"""Check DataField selection export rejects ranges too large for legacy APIs."""
from pathlib import Path

source = Path("lib/Xm/DataFSel.c").read_text(encoding="utf-8")
assert "text_selection_span_to_int" in source
assert "length > (XmTextPosition)INT_MAX" in source
assert source.count("text_selection_span_to_int(left, right, &selection_span)") == 3
assert "(int)(right - left)" not in source
print("PASS DataField selection exports validate position spans before narrowing")
