#!/usr/bin/env python3
"""Check Mrm compression-table fixup rejects invalid table inputs."""
from pathlib import Path

source = Path(__file__).parents[2] / "lib" / "Mrm" / "Mrmtable.c"
text = source.read_text(encoding="utf-8")
start = text.index("Urm__FixupCompressionTable (UidCompressionTablePtr")
end = text.index("Urm__FindClassDescriptor (IDBFile", start)
block = text[start:end]
for needle in ("if ( ctable == NULL )", "ctable->num_entries < UilMrmMinValidCode"):
    if needle not in block:
        raise SystemExit(f"missing compression-table fixup guard: {needle}")
if block.index("ctable->num_entries < UilMrmMinValidCode") > block.index("for ( fixndx="):
    raise SystemExit("compression-table size guard occurs after entry walk")
print("PASS Mrm compression-table fixup rejects null and undersized tables")
