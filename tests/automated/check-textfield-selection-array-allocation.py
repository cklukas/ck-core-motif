#!/usr/bin/env python3
"""Check TextField selection-array replacement is transactional."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "TextF.c").read_text()
start = source.index("if (TextF_SelectionArray(new_tf) != TextF_SelectionArray(old_tf))")
end = source.index("  /*\n   * End fix for HaL DTS 9841", start)
body = source[start:end]

assert "selection_count > SIZE_MAX / sizeof(XmTextScanType)" in body
assert "selection_count_value < 0" in body
assert "if (new_selection == NULL)" in body
assert body.index("if (new_selection == NULL)") < body.index("memcpy")
assert body.index("memcpy") < body.index("XtFree((char *)TextF_SelectionArray(old_tf));")
assert body.index("XtFree((char *)TextF_SelectionArray(old_tf));") < body.index("TextF_SelectionArray(new_tf) = new_selection;")
print("TextField selection-array replacement is transactional")
