#!/usr/bin/env python3
"""Guard GrabShell expose traversal against signed indices."""

from pathlib import Path

text = (Path(__file__).parents[2] / "lib" / "Xm" / "GrabShell.c").read_text(
    encoding="utf-8"
)
start = text.index("_XmFastExpose(")
body = text[start:]
if "composite.num_children" not in body:
    raise SystemExit("GrabShell expose no longer traverses child count")
if "register int i;" in body or "int i;" in body:
    raise SystemExit("GrabShell expose uses a signed child index")
if "register Cardinal i;" not in body:
    raise SystemExit("GrabShell expose is missing Cardinal index")

print("GrabShell child-count bounds checks passed")
