#!/usr/bin/env python3
"""Check SetValues preserves the old selection array on allocation failure."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib/Xm/DataF.c").read_text(
    encoding="utf-8"
)
start = source.index("if (XmTextF_selection_array(new_tf) !=")
end = source.index("/*\n * End fix for HaL DTS 9841", start)
context = source[start:end]
required = (
    "XmTextScanType *new_array;",
    "new_array = (XmTextScanType *)XtMalloc",
    "if (new_array != NULL)",
    "XtFree((char *)XmTextF_selection_array(old_tf));",
)
missing = [item for item in required if item not in context]
if missing:
    raise SystemExit("SetValues selection-array guard missing: " + ", ".join(missing))
if context.index("new_array =") > context.index("XtFree((char *)XmTextF_selection_array(old_tf))"):
    raise SystemExit("old selection array is freed before replacement allocation")

print("DataField SetValues selection-array checks passed")
