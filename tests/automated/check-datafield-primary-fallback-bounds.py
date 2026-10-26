#!/usr/bin/env python3
"""Check raw wide primary-selection length bounds before allocation."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib/Xm/DataF.c").read_text(
    encoding="utf-8"
)
start = source.index("if (*length > (unsigned long)INT_MAX")
end = source.index("wc_value = (wchar_t*)XtMalloc", start)
context = source[start:end]
required = (
    "*length > (unsigned long)INT_MAX",
    "UINT_MAX / sizeof(wchar_t)",
    "XmTextF_max_length(tf) = max_length;",
    "XtFree((char *)prim_select);",
)
missing = [item for item in required if item not in context]
if missing:
    raise SystemExit("primary fallback bounds missing: " + ", ".join(missing))

print("DataField primary-fallback-bound checks passed")
