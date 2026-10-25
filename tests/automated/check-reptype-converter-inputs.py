#!/usr/bin/env python3
"""Check that representation-type converters validate Xt inputs and records."""

from pathlib import Path


text = (Path(__file__).parents[2] / "lib" / "Xm" / "RepType.c").read_text(
    encoding="utf-8"
)
for name in ("static Boolean\nConvertRepType(",
             "static Boolean\nReverseConvertRepType("):
    start = text.index(name)
    body = text[start : text.index("}\n", start) + 2]
    if "args == NULL || args[0].addr == NULL" not in body:
        raise SystemExit(f"converter input guard is missing: {name}")
    if "if (Record == NULL)" not in body:
        raise SystemExit(f"converter record guard is missing: {name}")

print("representation-type converter guards present")
