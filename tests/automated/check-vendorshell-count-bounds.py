#!/usr/bin/env python3
"""Guard VendorShell argument and child traversals against signed counters."""

from pathlib import Path
import re

text = (Path(__file__).parents[2] / "lib" / "Xm" / "VendorS.c").read_text(
    encoding="utf-8"
)
for function, count in (
    ("SetValues", r"\*num_args"),
    ("GetValuesHook", r"\*num_args"),
    ("Resize", r"sw->composite\.num_children"),
):
    start = text.index(f"\n{function}(")
    body = text[text.index("{", start):text.index("\nstatic ", start)]
    if not re.search(rf"for\s*\(\s*i\s*=\s*0\s*;\s*i\s*<\s*{count}\b", body):
        raise SystemExit(f"{function} no longer traverses its counted inputs")
    if not re.search(r"\bCardinal\s+i\s*;", body):
        raise SystemExit(f"{function} is missing Cardinal counter")

print("VendorShell count bounds checks passed")
