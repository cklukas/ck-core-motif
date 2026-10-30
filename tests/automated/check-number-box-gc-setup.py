#!/usr/bin/env python3
"""Check NumberBox GC setup rolls back incomplete allocation."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "NumberBox.c").read_text(
    encoding="utf-8"
)
start = source.rindex("static void Initialize(")
end = source.index("\nstatic void Destroy", start)
region = source[start:end]
guard = region.index("if (number->number_box.text_gc == NULL")
for needle in (
    "XFreeGC(display, number->number_box.text_gc)",
    "XFreeGC(display, number->number_box.arrow_gc)",
    "XFreeGC(display, number->number_box.invalid_gc)",
    "number->number_box.text_gc = NULL;",
    "number->number_box.arrow_gc = NULL;",
    "number->number_box.invalid_gc = NULL;",
):
    if needle not in region[guard:]:
        raise SystemExit(f"NumberBox GC rollback missing: {needle}")

print("PASS NumberBox drawing GC setup rolls back incomplete allocation")
