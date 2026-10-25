#!/usr/bin/env python3
"""Check that trait updates remove the superseded binding first."""

from pathlib import Path


text = (Path(__file__).parents[2] / "lib" / "Xm" / "Trait.c").read_text(
    encoding="utf-8"
)
start = text.index("XmeTraitSet(")
body = text[start : text.index("}\n", start) + 2]
for needle in ("old_data = _XmGetHashEntry", "old_key = _XmRemoveHashEntry",
               "XtFree((char *)old_key);"):
    if needle not in body:
        raise SystemExit(f"trait replacement handling is missing: {needle}")
if body.index("old_key = _XmRemoveHashEntry") > body.index("_XmAddHashEntry(TraitTable, entry, data)"):
    raise SystemExit("trait replacement removes the old binding too late")

print("trait replacement handling present")
