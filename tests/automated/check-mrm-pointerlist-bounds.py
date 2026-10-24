#!/usr/bin/env python3
"""Check Mrm pointer-list capacities before allocation and doubling."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Mrm" / "Mrmptrlist.c"
text = source.read_text(encoding="utf-8")

init = text[text.index("UrmPlistInit ("):text.index("UrmPlistResize (")]
resize = text[text.index("UrmPlistResize ("):text.index("UrmPlistFree (")]
append = text[text.index("UrmPlistAppendPointer ("):text.index("UrmPlistAppendString (")]

for region, needles, label in (
    (
        init,
        (
            "size <= 0",
            "(size_t)size > SIZE_MAX / sizeof(XtPointer)",
            "*list_id_return = NULL",
        ),
        "initialization",
    ),
    (
        resize,
        (
            "size <= 0",
            "(size_t)size > SIZE_MAX / sizeof(XtPointer)",
        ),
        "resize",
    ),
    (
        append,
        (
            "num_ptrs >= list_id->num_slots",
            "num_slots <= 0",
            "num_slots > INT_MAX / 2",
        ),
        "append",
    ),
):
    for needle in needles:
        if needle not in region:
            raise SystemExit(f"Mrm pointer-list {label} guard missing: {needle}")

print("Mrm pointer-list capacity arithmetic is bounded")
