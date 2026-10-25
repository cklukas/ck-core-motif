#!/usr/bin/env python3
"""Check that dynamic representation-type growth fails closed."""

from pathlib import Path

source = Path(__file__).parents[2] / "lib" / "Xm" / "RepType.c"
text = source.read_text()
start = text.index("XmRepTypeRegister(")
end = text.index("void\nXmRepTypeAddReverse", start)
block = text[start:end]

if "new_dynamic_types =" not in block:
    raise SystemExit("representation-type growth does not use a temporary allocation")
if "if (new_dynamic_types == NULL)" not in block:
    raise SystemExit("representation-type growth has no allocation-failure guard")
if "return( XmREP_TYPE_INVALID);" not in block:
    raise SystemExit("representation-type allocation failure does not return invalid")
if "DynamicRepTypes = new_dynamic_types;" not in block:
    raise SystemExit("successful representation-type growth is not installed")

print("representation-type registration handles table allocation failure")
