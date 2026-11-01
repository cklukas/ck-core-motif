#!/usr/bin/env python3
"""Check Text hard-line scratch growth is bounded and failure-atomic."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib/Xm/Text.c").read_text(encoding="utf-8")
start = source.index("static Boolean\nGrowTemporaryLineTable")
end = source.index("\nunsigned int\n_XmTextGetTableIndex", start)
helper = source[start:end]
update_start = source.index("void \n_XmTextUpdateLineTable")
update_end = source.index("\n\n/*\n * Compare the old_highlight", update_start)
update = source[update_start:update_end]

assert "requested_capacity <= 0" in helper
assert "SIZE_MAX / sizeof(XmTextLineTableRec)" in helper
assert "UINT_MAX / sizeof(XmTextLineTableRec)" in helper
assert "resized = (XmTextLineTable)XtRealloc(" in helper
assert "if (resized == NULL)" in helper
assert "*table = resized;" in helper
assert update.count("GrowTemporaryLineTable(&temp_table, &temp_table_size,") == 2
assert update.count("temp_table_size > INT_MAX / 2") == 2
assert update.count("XtFree((char *)temp_table);") >= 4
assert update.count("tw->text.needs_refigure_lines = TRUE;") >= 4
assert "temp_table = (XmTextLineTable)XtRealloc" not in update
print("PASS Text hard-line scratch-table growth is failure-atomic")
