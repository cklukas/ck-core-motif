#!/usr/bin/env python3
"""Ensure accessibility text length counts UTF-8 scalars, not bytes."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib/Xm/Accessible.c").read_text()
start = source.index("static int bounded_text_length")
end = source.index("/* Live-object indexes", start)
body = source[start:end]
required = (
    "const unsigned char *p",
    "size_t remaining = text ? strlen(text) : 0;",
    "remaining >= 2",
    "remaining >= 3",
    "remaining >= 4",
    "p[1] >= 0x80",
    "p[2] >= 0x80",
    "p[3] >= 0x80",
    "if (count == (size_t)INT_MAX) return INT_MAX;",
)
missing = [item for item in required if item not in body]
if missing:
    raise SystemExit("accessible UTF-8 scalar counter incomplete: " + ", ".join(missing))
if "size_t length = text ? strlen(text) : 0" in body:
    raise SystemExit("accessible text length still returns UTF-8 byte length")

print("PASS accessibility text length counts UTF-8 scalars")
