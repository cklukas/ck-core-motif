#!/usr/bin/env python3
"""Check text drop location data has transfer lifetime storage."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "TextSel.c").read_text()
assert "XPoint *drop_point;" in source
assert "drop_point = (XPoint *) XtMalloc(sizeof(XPoint));" in source
assert "if (drop_point == NULL)" in source
assert "ds->location_data = (XtPointer) drop_point;" in source
location = source.index("ds->location_data = (XtPointer) drop_point;")
cleanup = source.index("XmeTransferAddDoneProc(ds->transfer_id, FreeLocationData);",
                       location)
drop_action = source.index("if (cb->dropAction != XmDROP_HELP)", location)
assert location < cleanup < drop_action
assert "(XtPointer) &DropPoint" not in source
print("PASS text drop location data has transfer lifetime storage")
