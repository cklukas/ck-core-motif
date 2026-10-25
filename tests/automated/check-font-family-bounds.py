#!/usr/bin/env python3
"""Check that wildcard font-family fallback copies into a bounded buffer."""
from pathlib import Path
source = (Path(__file__).parents[2] / "lib" / "Xm" / "FontS.c").read_text(encoding="utf-8")
start = source.index("String str = _XmGetMBStringFromXmString")
region = source[start:source.index("XtFree((XtPointer) str);", start)]
assert "strncpy(temp, str, BUFSIZ - 1);" in region
assert "temp[BUFSIZ - 1]" in region
assert "strcpy(temp, str);" not in region
print("PASS wildcard font-family fallback is bounded and terminated")
