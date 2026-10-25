#!/usr/bin/env python3
"""Check spin-box insertion guards count and string-table size overflow."""
from pathlib import Path

source = Path(__file__).parents[2] / "lib" / "Xm" / "SSpinB.c"
text = source.read_text(encoding="utf-8")
start = text.index("if (ssb_p->num_values == INT_MAX")
end = text.index("\n    new_nvalues =", start)
region = text[start:end]
assert "SIZE_MAX - sizeof(XmString)" in region
assert "ssb_p->num_values == INT_MAX" in region
assert "_XmAppUnlock(app);" in region
assert "return;" in region
print("spin-box item insertion bounds count and table size")
