#!/usr/bin/env python3
"""Check that tab-list proposal counters cannot wrap."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Xm" / "XmStringFunc.c"
text = source.read_text(encoding="utf-8")
start = text.index("XmStringTableProposeTablist(")
end = text.index("\n}\n", start)
region = text[start:end]

required = (
    "unsigned int\t\tj;",
    "_XmTabLCount(tl) == UINT_MAX",
    "if (j == UINT_MAX)",
)
missing = [item for item in required if item not in region]
if missing:
    raise SystemExit("tab-list count bounds missing: " + ", ".join(missing))

print("tab-list proposal counters are bounded")
