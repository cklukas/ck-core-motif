#!/usr/bin/env python3
"""Check Mrm child-list counts before descriptor sizing."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Mrm" / "Mrmwcrwr.c"
text = source.read_text(encoding="utf-8")
start = text.index("UrmCWRInitChildren (")
end = text.index("\n}\n", start)
region = text[start:end]
for needle in (
    "nchildren == 0",
    "nchildren > RGMListSizeMax",
    "sizeof(RGMChildrenDesc) > USHRT_MAX",
    "(size_t)(nchildren - 1)",
    "sizeof(RGMChildDesc)",
):
    if needle not in region:
        raise SystemExit(f"Mrm child-list guard missing: {needle}")

print("Mrm child-list descriptor sizing is bounded")
