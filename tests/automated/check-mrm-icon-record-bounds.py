#!/usr/bin/env python3
"""Check Mrm icon records before passing sizes to 16-bit storage."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Mrm" / "Mrmwcrwr.c"
text = source.read_text(encoding="utf-8")
start = text.index("case MrmRtypeIconImage:")
end = text.index("case MrmRtype", start + len("case MrmRtypeIconImage:"))
region = text[start:end]
for needle in (
    "icon_size = src_icon ? UrmIconImageSize(src_icon) : 0",
    "icon_size == 0",
    "icon_size > USHRT_MAX",
    "context_id, icon_size",
):
    if needle not in region:
        raise SystemExit(f"Mrm icon record guard missing: {needle}")

print("Mrm icon record sizes are bounded")
