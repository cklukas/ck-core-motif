#!/usr/bin/env python3
"""Check that trait hashing avoids signed pointer arithmetic."""

from pathlib import Path


text = (Path(__file__).parents[2] / "lib" / "Xm" / "Trait.c").read_text(
    encoding="utf-8"
)
start = text.index("static XmHashValue \nTraitHash")
body = text[start : text.index("}\n", start) + 2]
for needle in ("uintptr_t object", "uintptr_t name", "object ^ (object >> 16)"):
    if needle not in body:
        raise SystemExit(f"trait hash portability guard is missing: {needle}")
if "(long) entry -> obj" in body:
    raise SystemExit("trait hash still uses signed pointer arithmetic")

print("trait hash uses portable unsigned mixing")
