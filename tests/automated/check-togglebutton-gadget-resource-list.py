#!/usr/bin/env python3
"""Check ToggleButtonGadget class resource-list allocation guards."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ToggleBG.c").read_text()
start = source.index("static void \nClassInitialize( void )")
end = source.index("static void \nClassPartInitialize(", start)
region = source[start:end]

for needle in (
    "size_t                      merged_count;",
    "SIZE_MAX / sizeof(XtResource)",
    "UINT_MAX / sizeof(XtResource)",
    "merged_list = (XtResource *)XtMalloc",
    "if (merged_list == NULL)",
):
    assert needle in region, needle
assert region.index("if (merged_list == NULL)") < region.index("merged_list[i] =")
print("PASS ToggleButtonGadget resource-list allocation guard present")
