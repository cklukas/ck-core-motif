#!/usr/bin/env python3
"""Check XmStringGetNextSegment validates its context and outputs."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "XmStringSeg.c").read_text()
start = source.index("\nXmStringGetNextSegment(") + 1
end = source.index("\n}\n", start) + 3
region = source[start:end]
assert "if (context == NULL || text == NULL || tag == NULL" in region
assert "return(False);" in region
assert region.index("if (context == NULL") < region.index("_XmProcessLock")
print("PASS XmString next-segment input validation")
