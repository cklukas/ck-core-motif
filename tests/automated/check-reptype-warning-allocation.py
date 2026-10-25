#!/usr/bin/env python3
"""Check that reverse representation-type warning formatting handles OOM."""

from pathlib import Path

source = Path(__file__).parents[2] / "lib" / "Xm" / "RepType.c"
text = source.read_text()
start = text.index("ReverseConvertRepType(")
end = text.index("void\n_XmRepTypeInstallConverters", start)
block = text[start:end]

if "in_str = (char *) XtMalloc" not in block:
    raise SystemExit("reverse warning does not allocate its formatted text")
if "if (in_str == NULL)" not in block:
    raise SystemExit("reverse warning allocation has no null guard")
if "return(FALSE);" not in block:
    raise SystemExit("reverse warning allocation failure does not fail conversion")

print("reverse representation-type warning handles allocation failure")
