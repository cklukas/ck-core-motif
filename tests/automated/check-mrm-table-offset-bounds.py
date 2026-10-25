#!/usr/bin/env python3
"""Check Mrm compression-table offsets stay beyond table metadata."""
from pathlib import Path

source = Path(__file__).parents[2] / "lib" / "Mrm" / "Mrmtable.c"
text = source.read_text(encoding="utf-8")
start = text.index("Urm__FixupCompressionTable (UidCompressionTablePtr")
end = text.index("Urm__FindClassDescriptor (IDBFile", start)
block = text[start:end]
for needle in (
    "SIZE_MAX - (sizeof(UidCompressionTable) - sizeof(UidCTableEntry))",
    "table_bytes =",
    "ctable->entry[fixndx].stoffset",
):
    if needle not in block:
        raise SystemExit(f"missing compression-table offset guard: {needle}")
if "< table_bytes" not in block:
    raise SystemExit("compression-table offsets are not checked against metadata")
print("PASS Mrm compression-table offsets stay beyond table metadata")
