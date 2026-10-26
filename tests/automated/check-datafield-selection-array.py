#!/usr/bin/env python3
"""Check selection-array initialization restores its source on allocation failure."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib/Xm/DataF.c").read_text(
    encoding="utf-8"
)
start = source.index("df_Validates(")
start = source.index("XmTextF_selection_array(tf) = (XmTextScanType *)XtMalloc", start)
end = source.index("memcpy((void *)XmTextF_selection_array(tf)", start)
context = source[start:end]
required = (
    "if (XmTextF_selection_array(tf) == NULL)",
    "XmTextF_selection_array(tf) = (XmTextScanType *)temp_ptr;",
    "return;",
)
missing = [item for item in required if item not in context]
if missing:
    raise SystemExit("selection-array allocation guard missing: " + ", ".join(missing))

print("DataField selection-array checks passed")
