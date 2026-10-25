#!/usr/bin/env python3
"""Check merged CascadeButton resource-table sizing."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Xm" / "CascadeBG.c"
text = source.read_text(encoding="utf-8")
start = text.index("static void \nClassInitialize")
end = text.index("\nstatic ", start + 1)
region = text[start:end]
for needle in (
    "SIZE_MAX - (size_t)sc_num_res",
    "SIZE_MAX / sizeof(XtResource)",
    "if (merged_list == NULL)",
    "return;",
):
    if needle not in region:
        raise SystemExit(f"CascadeButton resource capacity guard missing: {needle}")

print("CascadeButton merged resource sizing is bounded")
