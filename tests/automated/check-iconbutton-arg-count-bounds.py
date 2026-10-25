#!/usr/bin/env python3
"""Guard IconButton SetValues argument scanning against signed counters."""

from pathlib import Path

text = (Path(__file__).parents[2] / "lib" / "Xm" / "IconButton.c").read_text(
    encoding="utf-8"
)
start = text.index("SetValues(Widget current", 10000)
end = text.index("\n}\n", start) + 2
body = text[start:end]
if "*num_args" not in body:
    raise SystemExit("IconButton SetValues no longer scans arguments")
if "register int i;" in body:
    raise SystemExit("IconButton SetValues uses a signed argument counter")
if "register Cardinal i;" not in body:
    raise SystemExit("IconButton SetValues is missing Cardinal counter")

print("IconButton argument-count bounds checks passed")
