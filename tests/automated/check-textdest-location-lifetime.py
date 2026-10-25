#!/usr/bin/env python3
"""Check text drop location data has transfer lifetime storage."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "TextSel.c").read_text()
assert "XPoint *drop_point;" in source
assert "drop_point = (XPoint *) XtMalloc(sizeof(XPoint));" in source
assert "if (drop_point == NULL)" in source
assert "ds->location_data = (XtPointer) drop_point;" in source
assert "ds->selection == atoms[XmA_MOTIF_DROP]" in source
assert "(XtPointer) &DropPoint" not in source
print("PASS text drop location data has transfer lifetime storage")
