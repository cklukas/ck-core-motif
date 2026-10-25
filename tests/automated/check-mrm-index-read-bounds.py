#!/usr/bin/env python3
"""Check Mrm read-side index traversal rejects malformed records safely."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Mrm" / "MrmIindex.c").read_text(encoding="utf-8")
search = source[source.index("\nIdb__INX_SearchIndex (") : source.index("\nCardinal ", source.index("\nIdb__INX_SearchIndex (") + 1)]
assert "MrmCount\t\tlowlim" in search
assert "if (*index_return == 0)" in search
assert "if (ndxcnt == 0)" in search
assert "Idb__INX_StringTerminated (stgbase, stg_offs" in search
assert "Idb__INX_StringTerminated" in source
resources = source[source.index("\nIdb__INX_FindResources (") :]
for guard in (
    "recno <= 0 || index_list == NULL",
    "ndxcnt == 0",
    "result = Idb__BM_GetRecord (file_id, recno, &bufptr)",
    "node_ndxvec[entndx].GT_record <= 0",
):
    assert guard in resources, guard
print("Mrm read-side index traversal validates counts, offsets, and children")
