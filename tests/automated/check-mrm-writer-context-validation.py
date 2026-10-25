#!/usr/bin/env python3
"""Check Mrm writer APIs validate resource contexts before dereferencing."""

import os
from pathlib import Path

source = Path(os.environ.get("MOTIF_ITOPW_SOURCE", Path(__file__).parents[2] / "lib" / "Mrm" / "MrmItopw.c"))
text = source.read_text(encoding="utf-8")
for name in ("UrmIdbPutIndexedResource (IDBFile", "UrmIdbPutRIDResource (IDBFile"):
    start = text.index(name)
    end = text.index("\n}", start)
    body = text[start:end]
    guard = body.index("context_id == NULL || !UrmRCValid(context_id)")
    first_deref = body.index("UrmRCGroup(context_id)")
    if guard > first_deref:
        raise SystemExit(f"{name} dereferences context before validation")
    if "return MrmFAILURE" not in body[guard:guard + 100]:
        raise SystemExit(f"{name} does not fail invalid contexts")
print("Mrm writer APIs validate resource contexts")
