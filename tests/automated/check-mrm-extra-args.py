#!/usr/bin/env python3
"""Check Mrm extra-argument counts before storing signed metadata."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Mrm" / "Mrmwcrwr.c"
text = source.read_text(encoding="utf-8")
start = text.index("UrmCWRSetExtraArgs (")
end = text.index("\n}\n", start)
region = text[start:end]
for needle in (
    "nextra > RGMListSizeMax",
    "Urm__UT_Error (\"UrmCWRSetExtraArgs\"",
    "MrmTOO_MANY",
):
    if needle not in region:
        raise SystemExit(f"Mrm extra-args guard missing: {needle}")

print("Mrm extra-argument counts are bounded")
