#!/usr/bin/env python3
"""Check DragBS treats XContext IDs as integer IDs, not pointers."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "DragBS.c").read_text()
assert "(XContext) NULL" not in source
assert "displayToAtomsContext = 0" in source
assert "displayToTargetsContext == 0" in source
print("PASS DragBS uses warning-free XContext zero checks")
