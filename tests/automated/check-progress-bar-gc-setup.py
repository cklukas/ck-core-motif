#!/usr/bin/env python3
"""Check ProgressBar GC setup rolls back an incomplete drawing pair."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "ProgressBar.c").read_text(
    encoding="utf-8"
)
start = source.rindex("static void Initialize(")
end = source.index("\nstatic void Destroy", start)
region = source[start:end]

guard = region.index("if (progress->progress.track_gc == NULL")
for needle in (
    "XFreeGC(display, progress->progress.track_gc)",
    "XFreeGC(display, progress->progress.fill_gc)",
    "XFreeGC(display, progress->progress.text_gc)",
    "progress->progress.track_gc = NULL;",
    "progress->progress.fill_gc = NULL;",
    "progress->progress.text_gc = NULL;",
):
    if needle not in region[guard:]:
        raise SystemExit(f"ProgressBar GC rollback missing: {needle}")

print("PASS ProgressBar drawing GC setup rolls back incomplete allocation")
