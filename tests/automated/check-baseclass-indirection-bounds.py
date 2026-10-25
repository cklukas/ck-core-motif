#!/usr/bin/env python3
"""Guard BaseClass resource indirection-table sizing."""

from pathlib import Path

text = (Path(__file__).parents[2] / "lib" / "Xm" / "BaseClass.c").read_text(
    encoding="utf-8"
)
start = text.index("CreateIndirectionTable(", 30000)
end = text.index("\n}\n", start) + 2
body = text[start:end]
for needle in (
    "register Cardinal i;",
    "SIZE_MAX / sizeof(*table)",
    "(size_t)num_resources * sizeof(*table)",
):
    if needle not in body:
        raise SystemExit(f"BaseClass indirection guard missing: {needle}")
if "register int i;" in body:
    raise SystemExit("BaseClass indirection table uses signed index")

print("BaseClass indirection bounds checks passed")
