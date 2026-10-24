#!/usr/bin/env python3
"""Check Mrm index traversal helpers validate pointer inputs."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Mrm" / "MrmIindex.c").read_text(encoding="utf-8")
regions = {
    "Idb__INX_FindIndex": "file_id == NULL || index == NULL || buffer_return == NULL",
    "Idb__INX_SearchIndex": "file_id == NULL || index == NULL || buffer == NULL",
    "Idb__INX_GetBtreeRecord": "file_id == NULL || buffer_return == NULL || *buffer_return == NULL",
}
for function, guard in regions.items():
    start = source.index("\n" + function + " (")
    next_start = source.find("\nCardinal \n", start + 1)
    region = source[start:] if next_start < 0 else source[start:next_start]
    assert guard in region, function
print("Mrm index traversal helpers validate pointer inputs")
