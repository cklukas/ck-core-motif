#!/usr/bin/env python3
from pathlib import Path

source = Path("lib/Xm/ScrolledW.c").read_text()
start = source.rindex("ScrollFrameInit (\n")
end = source.index("GetInfo(\n", start)
region = source[start:end]
assert "XtMalloc(sizeof(XmScrollFrameDataRec))" in region
assert "if (sw->swindow.scroll_frame_data == NULL)" in region
assert "sw->swindow.scroll_frame_inited = False;" in region
assert "return;" in region
print("ScrolledWindow scroll-frame allocation guard present")
