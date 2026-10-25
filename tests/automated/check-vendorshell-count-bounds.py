#!/usr/bin/env python3
"""Guard VendorShell argument and child traversals against signed counters."""

from pathlib import Path

text = (Path(__file__).parents[2] / "lib" / "Xm" / "VendorS.c").read_text(
    encoding="utf-8"
)
for function in ("SetValues(", "GetValuesHook(", "Resize("):
    start = text.index(function, 70000)
    end = text.find("\nstatic ", start + len(function))
    if end < 0:
        end = len(text)
    body = text[start:end]
    if function == "Resize(" and "composite.num_children" not in body:
        raise SystemExit("VendorShell Resize no longer traverses children")
    if function != "Resize(" and "*num_args" not in body:
        raise SystemExit(f"{function} no longer traverses arguments")
    if "int i;" in body:
        raise SystemExit(f"{function} uses a signed counter")
    if "Cardinal i;" not in body:
        raise SystemExit(f"{function} is missing Cardinal counter")

print("VendorShell count bounds checks passed")
