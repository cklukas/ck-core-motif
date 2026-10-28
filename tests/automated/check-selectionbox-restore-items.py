#!/usr/bin/env python3
"""Check SelectionBox restore validates its selected-item array."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "SelectioB.c").read_text()
start = source.index("_XmSelectionBoxRestore")
end = source.index("Widget \nXmSelectionBoxGetChild", start)
body = source[start:end]

assert "if(    count && items != NULL    )" in body
assert "_XmStringGetTextConcat( *items)" in body

print("SelectionBox restore guards missing selected-item data")
