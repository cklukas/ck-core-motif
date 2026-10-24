#!/usr/bin/env python3
"""Check legacy Mrm fetched font lists validate allocation counts."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Mrm" / "Mrmwvalues.c"
text = source.read_text(encoding="utf-8")
start = text.index("case MrmRtypeFontList:")
end = text.index("\n\tdefault:", start)
region = text[start:end]
count_pos = region.index("int count = ((OldRGMFontListPtr)val)->count;")
guard_pos = region.index("if (count <= 0", count_pos)
alloc_pos = region.index("XtMalloc", guard_pos)
if not (count_pos < guard_pos < alloc_pos):
    raise SystemExit("legacy font-list allocation is not guarded")
if "SIZE_MAX - sizeof(RGMFontList)" not in region[guard_pos:alloc_pos]:
    raise SystemExit("legacy font-list size-overflow guard is missing")
if "sizeof(RGMFontItem)" not in region[guard_pos:alloc_pos]:
    raise SystemExit("legacy font-list item-size check is missing")
if "XtFree ((char *)val)" not in region[guard_pos:alloc_pos]:
    raise SystemExit("invalid legacy font-list value is not released")
if "continue;" not in region[guard_pos:alloc_pos]:
    raise SystemExit("invalid legacy font-list value does not skip conversion")

print("Mrm fetched legacy font lists validate allocation bounds")
