#!/usr/bin/env python3
"""Check primary-selection wide conversion allocation cleanup."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib/Xm/DataF.c").read_text(
    encoding="utf-8"
)
start = source.index("wc_value = (wchar_t*)XtMalloc ((unsigned) malloc_size);")
end = source.index("for (i = 0; i < num_vals ; i++)", start)
context = source[start:end]
required = (
    "if (wc_value == NULL)",
    "XFreeStringList(tmp_value);",
    "XmTextF_max_length(tf) = max_length;",
    "XtFree((char *)prim_select);",
)
missing = [item for item in required if item not in context]
if missing:
    raise SystemExit("primary wide conversion guard missing: " + ", ".join(missing))

print("DataField primary-wide-conversion checks passed")
