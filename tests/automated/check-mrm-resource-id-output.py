#!/usr/bin/env python3
"""Check Mrm resource-id allocation validates its output pointer."""
from pathlib import Path

source = Path(__file__).parents[2] / "lib" / "Mrm" / "MrmItop.c"
text = source.read_text(encoding="utf-8")
start = text.index("UrmIdbGetResourceId (IDBFile")
end = text.index("Idb__FIL_Valid (IDBFile", start)
body = text[start:end]
needle = "if ( resource_id_return == NULL )"
if needle not in body or body.index(needle) > body.index("*resource_id_return ="):
    raise SystemExit("Mrm resource-id allocation lacks output guard")
print("PASS Mrm resource-id allocation validates its output pointer")
