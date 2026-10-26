#!/usr/bin/env python3
"""Check TextField overstrike scratch allocations before writes."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Xm" / "TextF.c"
text = source.read_text(encoding="utf-8")
start = text.index("static void\nPreeditDraw(")
end = text.index("\n}\n", start)
region = text[start:end]

for allocation, guard in (
    ("over_mb = XtMalloc(rest_len+1);", "if (over_mb == NULL)"),
    ("over_wc = (wchar_t *)XtMalloc((rest_len+1)*sizeof(wchar_t));",
     "if (over_wc == NULL)"),
):
    pos = region.find(allocation)
    if pos < 0 or region.find(guard, pos) < 0:
        raise SystemExit(f"missing overstrike allocation guard: {allocation}")
if region.count("if (mb == NULL)") < 3:
    raise SystemExit("not all TextField preedit byte allocations are guarded")
if region.count("if (wc == NULL)") < 3:
    raise SystemExit("not all TextField preedit wide allocations are guarded")

print("TextField overstrike allocations are failure-safe")
