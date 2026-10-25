#!/usr/bin/env python3
"""Check Mrm indexed-resource lookup rejects null index/context inputs."""
from pathlib import Path

source = Path(__file__).parents[2] / "lib" / "Mrm" / "MrmItop.c"
text = source.read_text(encoding="utf-8")
start = text.index("UrmIdbGetIndexedResource (IDBFile")
end = text.index("UrmIdbFindIndexedResource (IDBFile", start)
body = text[start:end]
needle = "index == NULL || context_id == NULL"
if needle not in body or body.index(needle) > body.index("Idb__FIL_Valid"):
    raise SystemExit("Mrm indexed-resource lookup lacks null-input guard")
print("PASS Mrm indexed-resource lookup rejects null index and context")
