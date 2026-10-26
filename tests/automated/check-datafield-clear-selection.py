#!/usr/bin/env python3
"""Check wide clear-selection spacing allocation failure handling."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib/Xm/DataF.c").read_text(
    encoding="utf-8"
)
start = source.index("df_ClearSelection( w, event")
end = source.index("df_PageRight( w, event", start)
context = source[start:end]
required = (
    "wc_spaces = (wchar_t *)XtMalloc",
    "if (wc_spaces == NULL)",
    "_XmDataFieldDrawInsertionPoint(tf, True);",
)
missing = [item for item in required if item not in context]
if missing:
    raise SystemExit("clear-selection allocation guard missing: " + ", ".join(missing))

print("DataField clear-selection checks passed")
