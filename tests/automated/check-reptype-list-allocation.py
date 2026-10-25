#!/usr/bin/env python3
"""Check that XmRepTypeGetRegistered handles its result allocation failure."""

from pathlib import Path

source = Path(__file__).parents[2] / "lib" / "Xm" / "RepType.c"
text = source.read_text()
start = text.index("XmRepTypeGetRegistered( void )")
end = text.index("XmRepTypeEntry\nXmRepTypeGetRecord", start)
block = text[start:end]

if "OutputList == NULL" not in block:
    raise SystemExit("registered representation-type list has no allocation guard")
if "_XmProcessUnlock();\n\treturn NULL;" not in block:
    raise SystemExit("registered representation-type list failure does not unlock and return")
if "UINT_MAX / sizeof(XmRepTypeEntryRec)" not in block:
    raise SystemExit("registered representation-type list lacks Xt size bound")
if "XtMalloc((Cardinal)((TotalEntries + 1) * sizeof(XmRepTypeEntryRec)))" not in block:
    raise SystemExit("registered representation-type list lacks explicit Xt sizing")

print("registered representation-type list handles allocation failure")
