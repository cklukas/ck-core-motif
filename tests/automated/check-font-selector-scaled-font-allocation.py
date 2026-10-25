#!/usr/bin/env python3
"""Guard FontSelector scaled-font fallback allocations."""

from pathlib import Path

text = (Path(__file__).parents[2] / "lib" / "Xm" / "FontS.c").read_text(
    encoding="utf-8"
)
start = text.index("\nDisplayCurrentFont(", 10000) + 1
body = text[start:text.index("\n/*\tFunction Name:", start)]
for needle in (
    "if (left_buf == NULL)",
    "if (right_buf == NULL)",
    "if (fbuf == NULL)",
):
    if needle not in body:
        raise SystemExit(f"missing scaled-font allocation guard: {needle}")
if "memcpy(left_buf, font" not in body:
    raise SystemExit("scaled-font prefix copy disappeared")

print("FontSelector scaled-font allocation checks passed")
