#!/usr/bin/env python3
"""Check that tab-group mutators validate widget inputs."""

from pathlib import Path


text = (Path(__file__).parents[2] / "lib" / "Xm" / "Traversal.c").read_text(
    encoding="utf-8"
)
for name, needle in (("XmAddTabGroup(", "tabGroup == NULL"),
                     ("XmRemoveTabGroup(", "w == NULL")):
    start = text.rfind(name)
    body = text[start : start + 500]
    if needle not in body:
        raise SystemExit(f"tab-group input guard is missing: {name}")

print("tab-group mutator guards present")
