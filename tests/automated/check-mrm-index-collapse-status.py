#!/usr/bin/env python3
"""Check Mrm index collapse failures reach split callers."""

from pathlib import Path

root = Path(__file__).parents[2]
source = (root / "lib" / "Mrm" / "MrmIindexw.c").read_text(encoding="utf-8")
header = (root / "lib" / "Mrm" / "Mrm.h").read_text(encoding="utf-8")
for name in ("Idb__INX_CollapseLeafRecord", "Idb__INX_CollapseNodeRecord"):
    assert f"Cardinal\n{name}" in source
    assert f"extern Cardinal {name}" in header
for call in (
    "result = Idb__INX_CollapseLeafRecord (lt_recptr, 0, lt_cnt-1)",
    "result = Idb__INX_CollapseLeafRecord (gt_recptr, p_index+1, p_index+gt_cnt)",
    "result = Idb__INX_CollapseNodeRecord (lt_recptr, 0, lt_cnt-1)",
    "result = Idb__INX_CollapseNodeRecord (gt_recptr, p_index+1, p_index+gt_cnt)",
):
    assert call in source
assert source.count("if (result != MrmSUCCESS) return result ;") >= 4
print("Mrm collapse failures propagate through both split paths")
