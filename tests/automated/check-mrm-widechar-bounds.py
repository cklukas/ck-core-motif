#!/usr/bin/env python3
"""Check signed Mrm wide-character counts before allocation."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Mrm" / "Mrmwcrw.c"
text = source.read_text(encoding="utf-8")
start = text.index("case MrmRtypeWideCharacter:")
end = text.index("case MrmRtypeFont:", start)
region = text[start:end]
for needle in (
    "wcharentry->wchar_item.count < 0",
    "max_size = (size_t)wcharentry->wchar_item.count",
    "max_size == SIZE_MAX",
    "SIZE_MAX / sizeof(wchar_t)",
):
    if needle not in region:
        raise SystemExit(f"Mrm wide-character count guard missing: {needle}")

print("Mrm wide-character counts are bounded")
