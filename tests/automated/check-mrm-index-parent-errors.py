#!/usr/bin/env python3
"""Check Mrm node insertion propagates both child-parent update failures."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Mrm" / "MrmIindexw.c").read_text(encoding="utf-8")
start = source.index("\nIdb__INX_EnterNodeIndex (")
end = source.index("\n\n\f", start)
region = source[start:end]
left = region.index("result = Idb__INX_SetParent (file_id, p_recno, lt_record)")
left_check = region.index("if ( result != MrmSUCCESS ) return result ;", left)
right = region.index("result = Idb__INX_SetParent (file_id, p_recno, gt_record)", left_check)
right_check = region.index("if ( result != MrmSUCCESS ) return result ;", right)
assert left_check < right < right_check
print("Mrm node insertion propagates left and right parent-update failures")
