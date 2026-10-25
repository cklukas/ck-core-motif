#!/usr/bin/env python3
"""Check deterministic resource-id output on Mrm failure paths."""

import os
from pathlib import Path

source = Path(os.environ.get("MOTIF_ITOP_SOURCE", Path(__file__).parents[2] / "lib" / "Mrm" / "MrmItop.c"))
text = source.read_text(encoding="utf-8")
start = text.index("UrmIdbGetResourceId (IDBFile")
end = text.index("\n}", start)
body = text[start:end]
local = body.index("IDBResource resource_id = 0;")
null_guard = body.index("if ( resource_id_return == NULL )")
clear = body.index("*resource_id_return = 0;", null_guard)
validity = body.index("Idb__FIL_Valid", clear)
if local > null_guard or clear > validity:
    raise SystemExit("resource-id output is initialized after a failure path")
if "*resource_id_return = resource_id;" not in body:
    raise SystemExit("successful resource-id result is not returned")
print("Mrm resource-id output is deterministic on failure")
