#!/usr/bin/env python3
"""Guard XmGetXmScreen child lookup against signed narrowing."""

from pathlib import Path

text = (Path(__file__).parents[2] / "lib" / "Xm" / "Screen.c").read_text(
    encoding="utf-8"
)
start = text.index("Widget \nXmGetXmScreen(")
end = text.index("\n}\n", start) + 2
body = text[start:end]
if "num_children = xmDisplay->composite.num_children" not in body:
    raise SystemExit("XmGetXmScreen child count assignment is missing")
if "int\tnum_children" in body or "int i;" in body:
    raise SystemExit("XmGetXmScreen narrows child count or index")
if "Cardinal num_children" not in body or "Cardinal i;" not in body:
    raise SystemExit("XmGetXmScreen is missing Cardinal types")

print("XmScreen child-count bounds checks passed")
