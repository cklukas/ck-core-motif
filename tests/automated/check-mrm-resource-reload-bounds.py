#!/usr/bin/env python3
"""Check Mrm resource traversal revalidates strings after node reload."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Mrm" / "MrmIindex.c").read_text(encoding="utf-8")
start = source.index("\nIdb__INX_FindResources (")
region = source[start:]
reload = region.rindex("result = Idb__BM_GetRecord (file_id, recno, &bufptr)")
check = region.index("Idb__INX_StringTerminated", reload)
record_check = region.index("if (!Idb__INX_ValidNode(bufptr))", reload)
assert record_check < check
assert "node_ndxvec[entndx].index_stg" in region[check:]
print("Mrm resource traversal revalidates node strings after reload")
