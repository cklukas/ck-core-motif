#!/usr/bin/env python3
"""Check that XmRepTypeGetRecord handles its output allocation failure."""

from pathlib import Path

source = Path(__file__).parents[2] / "lib" / "Xm" / "RepType.c"
text = source.read_text()
start = text.index("XmRepTypeGetRecord(")
end = text.index("XmRepTypeId\nXmRepTypeGetId", start)
block = text[start:end]

if "if (OutputRecord == NULL)" not in block:
    raise SystemExit("representation-type record has no allocation guard")
if "_XmProcessUnlock();\n\t      return( NULL);" not in block:
    raise SystemExit("representation-type record failure does not unlock and return")

print("representation-type record handles allocation failure")
