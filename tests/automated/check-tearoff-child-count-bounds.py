#!/usr/bin/env python3
"""Guard TearOff composite child traversals against signed indices."""

from pathlib import Path

text = (Path(__file__).parents[2] / "lib" / "Xm" / "TearOff.c").read_text(
    encoding="utf-8"
)

for function in (
    "_XmAddTearOffEventHandlers",
    "RemoveTearOffEventHandlers",
    "_XmRestoreTearOffToMenuShell",
):
    start = text.index("\n" + function + "(") + 1
    candidates = [p for p in (text.find("\nvoid ", start), text.find("\nstatic ", start)) if p != -1]
    next_start = min(candidates) if candidates else len(text)
    body = text[start:next_start]
    if "composite.num_children" not in body:
        raise SystemExit(f"{function} no longer traverses child count")
    if "int i;" in body:
        raise SystemExit(f"{function} uses a signed child index")

print("TearOff child-count bounds checks passed")
