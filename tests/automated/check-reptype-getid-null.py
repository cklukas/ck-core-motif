#!/usr/bin/env python3
"""Check that XmRepTypeGetId rejects a missing name before strcmp."""

from pathlib import Path


text = (Path(__file__).parents[2] / "lib" / "Xm" / "RepType.c").read_text(
    encoding="utf-8"
)
start = text.index("XmRepTypeGetId(")
body = text[start : text.index("}\n", start) + 2]
guard = "if (rep_type_name == NULL)\n\treturn XmREP_TYPE_INVALID;"
if guard not in body:
    raise SystemExit("rep-type name guard is missing")
if body.index(guard) > body.index("_XmProcessLock();"):
    raise SystemExit("rep-type name is checked too late")

print("rep-type null-name guard present")
