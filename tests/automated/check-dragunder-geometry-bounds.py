#!/usr/bin/env python3
"""Check DragUnder single-region geometry cannot wrap on border subtraction."""
from pathlib import Path

source = Path(__file__).resolve().parents[2] / "lib" / "Xm" / "DragUnder.c"
text = source.read_text(encoding="utf-8")
if text.count("(int)extents.width <= (int)(offset << 1)") != 2:
    raise SystemExit("DragUnder width bounds are not guarded in both draw paths")
if text.count("(Dimension)((int)extents.width - (int)(offset << 1))") != 2:
    raise SystemExit("DragUnder width subtraction is not signed and explicit")
if text.count("(Dimension)((int)extents.height - (int)(offset << 1))") != 2:
    raise SystemExit("DragUnder height subtraction is not signed and explicit")
print("DragUnder single-region geometry bounds are guarded")
