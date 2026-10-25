#!/usr/bin/env python3
"""Check Mrm indexed-resource enumeration validates its output list."""
from pathlib import Path

source = Path(__file__).parents[2] / "lib" / "Mrm" / "MrmItop.c"
text = source.read_text(encoding="utf-8")
start = text.index("UrmIdbFindIndexedResource (IDBFile")
end = text.index("UrmIdbGetRIDResource (IDBFile", start)
body = text[start:end]
needle = "if ( index_list == NULL )"
if needle not in body or body.index(needle) > body.index("Idb__FIL_Valid"):
    raise SystemExit("Mrm indexed-resource enumeration lacks output guard")
print("PASS Mrm indexed-resource enumeration validates its output list")
