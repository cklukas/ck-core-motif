#!/usr/bin/env python3
"""Check that deferred clipboard callbacks validate pointers and keys."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Transfer.c").read_text()
start = source.index("static void \nClipboardCallback")
end = source.index("/****************************************************************/\n/* XmeDragSource", start)
region = source[start:end]
assert "reason == NULL" in region
assert "target == NULL" in region
assert "req -> outstanding <= 0" in region
assert "_XmRemoveHashEntry(DataIdDictionary, (XmHashKey) *data_id)" in region
print("clipboard callbacks validate inputs and remove the matching data-id key")
