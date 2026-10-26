#!/usr/bin/env python3
"""Check that selection-loss callbacks validate widget and atom inputs."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Transfer.c").read_text()
for name in ("ClipboardLoseProc", "LoseProc"):
    start = source.index("static void\n" + name + "(Widget w")
    end = source.index("\n/****", start)
    region = source[start:end]
    assert "w == NULL || selection == NULL || *selection == None" in region
    assert region.index("if (w == NULL") < region.index("XtDisplay(w)")
print("selection-loss callbacks validate widget and atom inputs")
