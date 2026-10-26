#!/usr/bin/env python3
"""Check primary-selection narrow conversion allocation cleanup."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib/Xm/DataF.c").read_text(
    encoding="utf-8"
)
start = source.index("total_tmp_value = XtMalloc ((unsigned) malloc_size);")
end = source.index("total_tmp_value[0]", start)
context = source[start:end]
required = (
    "if (total_tmp_value == NULL)",
    "XFreeStringList(tmp_value);",
    "XmTextF_max_length(tf) = max_length;",
    "XtFree((char *)prim_select);",
)
missing = [item for item in required if item not in context]
if missing:
    raise SystemExit("primary conversion guard missing: " + ", ".join(missing))

print("DataField primary-conversion checks passed")
