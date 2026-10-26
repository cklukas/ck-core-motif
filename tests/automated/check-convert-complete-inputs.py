#!/usr/bin/env python3
"""Check null-safe conversion completion and destination lookup helpers."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Transfer.c").read_text()
start = source.index("_XmConvertComplete(")
end = source.index("/* Error handler for XGetAtomName", start)
region = source[start:end]
assert "if (cs == NULL)" in region
assert "XtFree((char *) value);" in region
assert "if (tc == NULL)" in region
assert "return NULL;" in region
print("conversion completion and destination lookup reject null state")
