#!/usr/bin/env python3
"""Check raw wide primary-selection fallback allocation cleanup."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib/Xm/DataF.c").read_text(
    encoding="utf-8"
)
start = source.index("(*length * sizeof(wchar_t))")
end = source.index("prim_select->num_chars = mbstowcs", start)
context = source[start:end]
required = (
    "if (wc_value == NULL)",
    "XmTextF_max_length(tf) = max_length;",
    "XtFree((char *)value);",
    "XtFree((char *)prim_select);",
)
missing = [item for item in required if item not in context]
if missing:
    raise SystemExit("primary fallback guard missing: " + ", ".join(missing))

print("DataField primary-fallback checks passed")
