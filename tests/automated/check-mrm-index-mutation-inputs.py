#!/usr/bin/env python3
"""Check Mrm index mutation helpers validate record inputs."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Mrm" / "MrmIindexw.c").read_text(encoding="utf-8")
checks = {
    "Idb__INX_SplitLeafRecord": "file_id == NULL || gt_buffer == NULL",
    "Idb__INX_SplitNodeRecord": "file_id == NULL || gt_buffer == NULL",
    "Idb__INX_ConfirmNodeSpace": "file_id == NULL || buffer == NULL || buffer->IDB_record == NULL",
    "Idb__INX_SetParent": "file_id == NULL || parent_record <= 0 || child_record <= 0",
}
for function, guard in checks.items():
    start = source.index("\n" + function + " (")
    next_start = source.find("\nCardinal\n", start + 1)
    region = source[start:] if next_start < 0 else source[start:next_start]
    if function == "Idb__INX_ConfirmNodeSpace":
        assert "file_id == NULL || ! Idb__INX_ValidNode(buffer)" in region, function
    else:
        assert guard in region, function
print("Mrm index mutation helpers validate record inputs")
