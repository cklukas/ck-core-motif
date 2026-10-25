#!/usr/bin/env python3
"""Check Mrm compression helpers reject null inputs and entries."""
from pathlib import Path

source = Path(__file__).parents[2] / "lib" / "Mrm" / "Mrmtable.c"
text = source.read_text(encoding="utf-8")
find = text[text.index("Urm__FindClassDescriptor (IDBFile"):
            text.index("Urm__UncompressCode (IDBFile")]
uncompress = text[text.index("Urm__UncompressCode (IDBFile"):
                 text.index("Urm__IsSubtreeResource (IDBFile")]
subtree = text[text.index("Urm__IsSubtreeResource (IDBFile"):]
if "class_return == NULL" not in find or "cfile == NULL" not in find:
    raise SystemExit("class lookup lacks null-input guards")
for needle in ("cfile == NULL || stg_return == NULL", "*stg_return == NULL"):
    if needle not in uncompress:
        raise SystemExit(f"compression lookup lacks guard: {needle}")
for needle in ("cfile == NULL || cfile->resource_ctable == NULL",
               "entry[code].cstring == NULL"):
    if needle not in subtree:
        raise SystemExit(f"subtree lookup lacks guard: {needle}")
print("PASS Mrm compression lookups reject null inputs and entries")
