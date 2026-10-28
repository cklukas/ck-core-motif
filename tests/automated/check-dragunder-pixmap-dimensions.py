#!/usr/bin/env python3
"""Check DragUnder saved pixmaps retain Motif Dimension widths."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "DragUnderI.h").read_text()
start = source.index("typedef struct _DragPixmapData")
end = source.index("} DragPixmapData;", start)
body = source[start:end]

assert "Dimension\twidth, height;" in body
assert "unsigned int\twidth, height;" not in body

print("DragUnder pixmap dimensions match the Motif ABI")
