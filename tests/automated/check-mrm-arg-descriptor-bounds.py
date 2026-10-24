#!/usr/bin/env python3
"""Check Mrm argument-list descriptor sizing before record allocation."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Mrm" / "Mrmwcrwr.c"
text = source.read_text(encoding="utf-8")
start = text.index("UrmCWRInitArglist (")
end = text.index("\n}\n", start)
region = text[start:end]
for needle in (
    "nargs == 0",
    "nargs > RGMListSizeMax",
    "sizeof(RGMArgListDesc) > USHRT_MAX",
    "(size_t)(nargs - 1)",
    "sizeof(RGMArgument)",
    "size_t\t\t\tdescsiz",
):
    if needle not in region:
        raise SystemExit(f"Mrm arg-list guard missing: {needle}")

print("Mrm argument-list descriptor sizing is bounded")
