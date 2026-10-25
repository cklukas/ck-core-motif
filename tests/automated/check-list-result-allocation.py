#!/usr/bin/env python3
"""Check List position result APIs handle allocation failure."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "List.c").read_text()

match_start = source.index("XmListGetMatchPos(")
match_end = source.index("XmListGetSelectedPos(", match_start)
match_region = source[match_start:match_end]
assert "pos = (int *)XtMalloc" in match_region
assert "if (pos == NULL)" in match_region
assert match_region.index("if (pos == NULL)") < match_region.index("j = 0;")

selected_start = source.index("XmListGetSelectedPos(")
selected_end = source.index("\n}\n", selected_start) + 3
selected_region = source[selected_start:selected_end]
assert "posList = (int *) XtMalloc" in selected_region
assert "if (posList == NULL)" in selected_region
assert selected_region.index("if (posList == NULL)") < selected_region.index("memcpy")
print("PASS List position result allocation failures are handled")
