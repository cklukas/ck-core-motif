#!/usr/bin/env python3
"""Check that Label pixmap drag-icon creation validates its widget input."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Xm" / "LabelG.c"
text = source.read_text(encoding="utf-8")
needle = """GetPixmapDragIcon(Widget w)
{
    XmLabelGadget lw;
    if (w == NULL)
        return NULL;
"""
if needle not in text:
    raise SystemExit("Label drag-icon input guard is missing")

print("Label pixmap drag-icon input guard present")
