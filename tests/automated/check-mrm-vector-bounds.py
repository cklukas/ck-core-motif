#!/usr/bin/env python3
"""Check signed vector counts before Mrm SafeCopyValue allocations."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Mrm" / "Mrmwcrw.c"
text = source.read_text(encoding="utf-8")
start = text.index("void \nUrm__CW_SafeCopyValue (")
region = text[start:]
for needle in (
    "vec_count < 0 || vec_size < 0",
    "(size_t)vec_count > SIZE_MAX / sizeof(XmString)",
    "return;",
):
    if needle not in region:
        raise SystemExit(f"Mrm vector capacity guard missing: {needle}")

print("Mrm vector copy sizing is bounded")
