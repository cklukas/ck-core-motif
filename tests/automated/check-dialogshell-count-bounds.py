#!/usr/bin/env python3
"""Guard DialogShell popup cleanup counters against signed indices."""

from pathlib import Path

text = (Path(__file__).parents[2] / "lib" / "Xm" / "DialogS.c").read_text(
    encoding="utf-8"
)
start = text.index("CHILD BEING UNMANAGED")
end = text.index("/* End Fix CR5043", start)
body = text[start:end]
if "core.num_popups" not in body or "composite.num_children" not in body:
    raise SystemExit("DialogShell cleanup list traversals are missing")
if "int i, j;" in body:
    raise SystemExit("DialogShell cleanup uses signed counters")
if "Cardinal i, j;" not in body:
    raise SystemExit("DialogShell cleanup is missing Cardinal counters")

print("DialogShell count bounds checks passed")
