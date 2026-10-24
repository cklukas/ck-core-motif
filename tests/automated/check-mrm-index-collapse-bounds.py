#!/usr/bin/env python3
"""Check Mrm index collapse helpers reject malformed record ranges."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Mrm" / "MrmIindexw.c").read_text(encoding="utf-8")
checks = {
    "Idb__INX_CollapseLeafRecord": (
        "recptr == NULL || end < start",
        "start >= hdrptr->index_count || end >= hdrptr->index_count",
        "strnlen (ndxstg",
    ),
    "Idb__INX_CollapseNodeRecord": (
        "recptr == NULL || end < start",
        "start >= hdrptr->index_count || end >= hdrptr->index_count",
        "strnlen (ndxstg",
    ),
}
for function, guards in checks.items():
    start = source.index("\n" + function + " (")
    next_void = source.find("\nvoid ", start + 1)
    next_cardinal = source.find("\nCardinal ", start + 1)
    candidates = [pos for pos in (next_void, next_cardinal) if pos >= 0]
    end = min(candidates) if candidates else len(source)
    region = source[start:end]
    for guard in guards:
        assert guard in region, (function, guard)
print("Mrm index collapse helpers validate ranges and string bounds")
