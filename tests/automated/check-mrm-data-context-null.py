#!/usr/bin/env python3
"""Check that the shared Mrm data-entry reader guards null contexts."""

from pathlib import Path

source = Path(__file__).parents[2] / "lib" / "Mrm" / "MrmIentry.c"
text = source.read_text(encoding="utf-8")
start = text.index("Idb__DB_GetDataEntry (IDBFile")
end = text.index("\n}", start)
body = text[start:end]
guard = body.index("context_id == NULL || ! UrmRCValid (context_id)")
validation = body.index("UrmRCValid (context_id)", guard)
if guard > validation:
    raise SystemExit("context validation occurs before the null guard")
if "return Urm__UT_Error" not in body[guard:guard + 190]:
    raise SystemExit("null context does not return the bad-context error")
print("PASS Mrm data-entry reader guards null contexts")
