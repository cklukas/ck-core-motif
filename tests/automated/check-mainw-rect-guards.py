#!/usr/bin/env python3
"""Check MainWindow auto-scroll rectangle allocation is guarded."""

from pathlib import Path

source = Path(__file__).parents[2] / "lib" / "Xm" / "MainW.c"
text = source.read_text()
start = text.index("static void \nGetVertRects")
end = text.index("/************************************************************************", start + 20)
block = text[start:end]

if "*num_vrect = 0" not in block or "*vrect = NULL" not in block:
    raise SystemExit("rectangle outputs are not initialized to empty")
if "if (*vrect == NULL)" not in block:
    raise SystemExit("rectangle allocation is not guarded")

print("MainWindow auto-scroll rectangle allocation is guarded")
