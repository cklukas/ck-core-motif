#!/usr/bin/env python3
"""Check Mrm text-vector counts before 16-bit record sizing."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Mrm" / "Mrmwcrwr.c"
text = source.read_text(encoding="utf-8")
for function, count in (
    ("UrmCWRSetArgChar8Vec", "num_stg"),
    ("UrmCWRSetArgCStringVec", "num_cstg"),
):
    start = text.index(function + " (")
    end = text.index("\n}\n", start)
    region = text[start:end]
    for needle in (
        f"{count} < 0",
        f"{count} > RGMListSizeMax",
        "sizeof(RGMTextVector) > USHRT_MAX",
        f"(size_t){count} >",
        "sizeof(RGMTextEntry)",
    ):
        if needle not in region:
            raise SystemExit(f"{function} guard missing: {needle}")

print("Mrm text-vector counts are bounded")
