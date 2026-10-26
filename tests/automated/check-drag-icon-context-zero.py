#!/usr/bin/env python3
"""Check DragIcon treats its XContext ID as an integer ID."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "DragIcon.c").read_text()
assert "(XContext) NULL" not in source
assert "_XmTextualDragIconContext = 0" in source
assert "_XmTextualDragIconContext == 0" in source
print("PASS DragIcon uses warning-free XContext zero checks")
