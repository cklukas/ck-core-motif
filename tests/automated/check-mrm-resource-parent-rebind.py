#!/usr/bin/env python3
"""Check Mrm resource traversal rebinds the parent before each node entry."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Mrm" / "MrmIindex.c").read_text(encoding="utf-8")
start = source.index("\nIdb__INX_FindResources (")
region = source[start:]
node = region.index("case IDBrtIndexNode:")
loop = region.index("for ( entndx=0 ; entndx<ndxcnt ; entndx++ )", node)
rebind = region.index("Idb__BM_GetRecord (file_id, recno, &bufptr)", loop)
string_check = region.index("Idb__INX_StringTerminated", loop)
entry_read = region.index("entry_data.rec_no", loop)
assert rebind < string_check < entry_read
print("Mrm resource traversal rebinds the parent before reading each entry")
