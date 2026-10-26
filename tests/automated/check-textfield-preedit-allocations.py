#!/usr/bin/env python3
"""Check that TextField preedit scratch allocations are failure-safe."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Xm" / "TextF.c"
text = source.read_text(encoding="utf-8")
start = text.index("static void\nPreeditDraw(")
end = text.index("\n}\n", start)
region = text[start:end]

for allocation, guard in (
    ("tab_wc = (wchar_t*) XtMalloc", "if (tab_wc == NULL)"),
    ("mb = XtMalloc((insert_length+1)", "if (mb == NULL)"),
    ("mb = XtMalloc(4);", "if (mb == NULL)"),
    ("wc = (wchar_t *) XtMalloc", "if (wc == NULL)"),
):
    pos = region.find(allocation)
    if pos < 0 or region.find(guard, pos) < 0:
        raise SystemExit(f"missing allocation guard after {allocation}")

print("TextField preedit scratch allocations are failure-safe")
