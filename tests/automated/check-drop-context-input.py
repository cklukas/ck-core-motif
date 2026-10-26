#!/usr/bin/env python3
"""Check that destination setup rejects an incomplete drop context."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Transfer.c").read_text()
start = source.index("_XmDestinationHandler(")
end = source.index("/**************************************************************/\n/* XmeClipboardSink", start)
region = source[start:end]
assert "ds == NULL || ds -> dragContext == NULL" in region
assert "XtGetValues(ds->dragContext" in region
print("drop destination setup validates its drag context")
