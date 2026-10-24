#!/usr/bin/env python3
"""Check defined ctype inputs in picture matching."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Xm" / "Picture.c"
text = source.read_text(encoding="utf-8")

for name in ("isdigit", "isxdigit", "isalpha", "islower", "isalnum", "toupper"):
    if f"{name}((unsigned char)" not in text:
        raise SystemExit(f"picture ctype call is not unsigned: {name}")

print("picture ctype conversion is defined for all bytes")
