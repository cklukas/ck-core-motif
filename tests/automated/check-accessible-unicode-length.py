#!/usr/bin/env python3
"""Ensure accessibility text length counts UTF-8 scalars, not bytes."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib/Xm/Accessible.c").read_text()
start = source.index("static int bounded_text_length")
end = source.index("/* Live-object indexes", start)
body = source[start:end]
required = (
    "const unsigned char *p",
    "p[1] >= 0x80",
    "p[2] >= 0x80",
    "p[3] >= 0x80",
    "if (count == (size_t)INT_MAX) return INT_MAX;",
)
missing = [item for item in required if item not in body]
if missing:
    raise SystemExit("accessible UTF-8 scalar counter incomplete: " + ", ".join(missing))
if "strlen(text)" in body:
    raise SystemExit("accessible text length still counts UTF-8 bytes")

print("PASS accessibility text length counts UTF-8 scalars")
