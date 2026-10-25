#!/usr/bin/env python3
"""Check IconGadget detail-table copying is bounded and transactional."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "IconG.c").read_text()

start = source.index("static XmStringTable\nCopyDetailTable(XmStringTable source")
end = source.index("\n}\n", start) + 2
helper = source[start:end]
assert "SIZE_MAX / sizeof(*copy)" in helper
assert "UINT_MAX / sizeof(*copy)" in helper
assert "if (copy[i] == NULL)" in helper
assert "while (i != 0)" in helper
assert "XmStringFree(copy[--i])" in helper
assert "XtFree((char *)copy)" in helper

set_start = source.index("if (IG_Detail(nw) != IG_Detail(cw))")
set_end = source.index("\n    if (IG_DetailCount(nw) != IG_DetailCount(cw))", set_start)
set_values = source[set_start:set_end]
assert "Copy before releasing the current table." in set_values
assert "detail = CopyDetailTable" in set_values
assert "Keep the previous value if the requested copy failed." in set_values
assert "IG_Detail(nw) = IG_Detail(cw);" in set_values
print("IconGadget detail-table copies are bounded and transactional")
