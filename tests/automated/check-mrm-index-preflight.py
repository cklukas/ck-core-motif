#!/usr/bin/env python3
"""Check Mrm node insertion validates links before mutating the record."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Mrm" / "MrmIindexw.c").read_text(encoding="utf-8")
start = source.index("\nIdb__INX_EnterNodeIndex (")
end = source.index("\n\n\f", start)
region = source[start:end]
preflight = region.index("/* Validate neighboring child links before changing the node. */")
first_write = region.index("itemvec[ndx].index_stg =", preflight)
left_check = region.index("itemvec[entndx-1].GT_record != gt_record", preflight)
right_check = region.index("itemvec[entndx].LT_record != gt_record", preflight)
assert left_check < first_write
assert right_check < first_write
assert region.count('return Urm__UT_Error (\"Idb__INX_EnterNodeIndex\"') == 2
print("Mrm node insertion preflights neighboring links before mutation")
