#!/usr/bin/env python3
"""Check that Mrm close keeps file ownership after a decommit failure."""

import os
from pathlib import Path

source = Path(os.environ.get("MOTIF_ITOP_SOURCE", Path(__file__).parents[2] / "lib" / "Mrm" / "MrmItop.c"))
text = source.read_text(encoding="utf-8")
start = text.index("UrmIdbCloseFile (IDBFile")
end = text.index("\n}", start)
body = text[start:end]
for field in ("class_ctable", "resource_ctable"):
    released = body.index("XtFree ((char*)file_id->" + field + ")")
    cleared = body.index("file_id->" + field + " = NULL", released)
    if cleared < released:
        raise SystemExit(f"{field} is cleared before it is released")
decommit = body.index("result = Idb__BM_DecommitAll (file_id)")
failure = body.index("if ( result != MrmSUCCESS )", decommit)
branch_end = body.index("\n    }", failure)
branch = body[failure:branch_end]
if "return result;" not in branch:
    raise SystemExit("decommit failure does not return while retaining the descriptor")
if "_error_close" in branch or "XtFree((char*)file_id)" in branch:
    raise SystemExit("decommit failure still frees the file descriptor")
print("Mrm close retains file ownership after decommit failure")
