#!/usr/bin/env python3
"""Check Mrm RID-resource lookup validates its context."""
from pathlib import Path

source = Path(__file__).parents[2] / "lib" / "Mrm" / "MrmItop.c"
text = source.read_text(encoding="utf-8")
start = text.index("UrmIdbGetRIDResource (IDBFile")
end = text.index("UrmIdbGetResourceId (IDBFile", start)
body = text[start:end]
needle = "if ( context_id == NULL )"
if needle not in body or body.index(needle) > body.index("Idb__FIL_Valid"):
    raise SystemExit("Mrm RID-resource lookup lacks context guard")
print("PASS Mrm RID-resource lookup validates its context")
