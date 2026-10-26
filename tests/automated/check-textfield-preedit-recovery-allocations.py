#!/usr/bin/env python3
"""Check TextField recovery buffers before conversion writes."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Xm" / "TextF.c"
text = source.read_text(encoding="utf-8")
start = text.index("static void\nPreeditDraw(")
end = text.index("\n}\n", start)
region = text[start:end]

for allocation, guard in (
    ("recover_mb = XtMalloc", "if (recover_mb == NULL)"),
    ("recover_wc = (wchar_t*) XtMalloc", "if (recover_wc == NULL)"),
):
    pos = region.find(allocation)
    if pos < 0 or region.find(guard, pos) < 0:
        raise SystemExit(f"missing recovery allocation guard: {allocation}")

print("TextField recovery buffers are checked before conversion")
