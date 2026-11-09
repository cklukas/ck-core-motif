#!/usr/bin/env python3
"""Check ToggleButton records saturated indicator margin deltas."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ToggleB.c").read_text(
    encoding="utf-8"
)
start = source.index("static void \nInitialize(")
end = source.index("static void \nDestroy(", start)
body = source[start:end]

for name in ("Top", "Bottom", "Right", "Left"):
    assert f"new_w->toggle.ind_{name.lower()}_delta = new_margin - old_margin;" in body
assert body.count("toggle_bounded_dimension(\n") >= 4
print("PASS ToggleButton indicator margins record saturated deltas")
