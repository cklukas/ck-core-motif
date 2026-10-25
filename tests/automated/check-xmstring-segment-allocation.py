#!/usr/bin/env python3
"""Check that XmString segment insertion handles allocation failure."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "XmString.c").read_text(encoding="utf-8")
start = source.index("void \n_XmStringSegmentNew(")
end = source.index("static _XmString", start)
region = source[start:end]

assert "if (new_entries == NULL)" in region
assert "if (new_segments == NULL)" in region
assert region.count("if (copy &&") >= 2
assert "SIZE_MAX / sizeof(_XmStringEntry)" in region
assert "UINT_MAX / sizeof(_XmStringEntry)" in region
assert "(Cardinal)(sizeof(_XmStringEntry) * (size_t)(sc + 1))" in region
assert "sc == INT_MAX" in region
print("PASS XmString segment allocation failures are guarded")
