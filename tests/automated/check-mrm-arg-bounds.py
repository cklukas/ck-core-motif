#!/usr/bin/env python3
"""Check Mrm Arg-list count and allocation bounds."""

from pathlib import Path


for name in ("Mrmwvalues.c", "Mrmwcrw.c"):
    text = (Path(__file__).parents[2] / "lib" / "Mrm" / name).read_text(encoding="utf-8")
    if "SIZE_MAX / sizeof(Arg)" not in text:
        raise SystemExit(f"{name}: Arg allocation guard missing")
    if name == "Mrmwcrw.c" and text.count("SIZE_MAX - (size_t)num_listent") < 2:
        raise SystemExit(f"{name}: count addition guards missing")

print("Mrm Arg-list sizing is bounded")
