#!/usr/bin/env python3
"""Check Mrm writers propagate lookup failures before creating records."""

import os
from pathlib import Path

source = Path(os.environ.get("MOTIF_ITOPW_SOURCE", Path(__file__).parents[2] / "lib" / "Mrm" / "MrmItopw.c"))
text = source.read_text(encoding="utf-8")
checks = (("Idb__INX_FindIndex", ("MrmINDEX_GT", "MrmINDEX_LT")),
          ("Idb__RID_ReturnItem", ("MrmNULL_DATA",)))
for lookup, absent_statuses in checks:
    start = text.index(lookup, text.index("UrmIdbPut"))
    # Use the first writer occurrence containing this lookup.
    end = text.index("\n}", start)
    body = text[start:end]
    success = body.index("if ( result == MrmSUCCESS)")
    guard = body.index("if ( result !=", success)
    for status in absent_statuses:
        if status not in body[guard:guard + 100]:
            raise SystemExit(f"{lookup} does not preserve {status} as its only absence status")
    if "return result" not in body[guard:guard + 100]:
        raise SystemExit(f"{lookup} failure is not propagated")
print("Mrm writer lookup failures are propagated")
